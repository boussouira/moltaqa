#include "booksearchfilter.h"
#include "mainwindow.h"
#include "utils.h"
#include "modelenums.h"
#include "clheader.h"
#include "clconstants.h"
#include "clutils.h"
#include "libraryenums.h"
#include "mainwindow.h"
#include "bookreaderhelper.h"
#include "xmlutils.h"

#include <qsqlquery.h>
#include <qitemselectionmodel.h>
#include <qdatetime.h>

BookSearchFilter::BookSearchFilter(QObject *parent) :
    SearchFilterManager(parent),
    m_book(0)
{
    setChangeFilterColumn(false);
    setAutoSelectParent(false);
}

BookSearchFilter::~BookSearchFilter()
{
}

void BookSearchFilter::setLibraryBook(LibraryBookPtr book)
{
    m_book = book;
}

void BookSearchFilter::loadModel()
{
    ml_delete_check(m_model);

    QStandardItemModel *model = new QStandardItemModel();
    model->setHorizontalHeaderLabels(QStringList() << tr("الفهرس"));

    open();

    if(m_book->isQuran())
        loadQuranModel(model);
    else
        loadSimpleBookModel(model);

    close();

    setSourceModel(model);
}

SearchFilter *BookSearchFilter::getFilterQuery()
{
    if(m_book->isQuran())
        return getQuranFilterQuery();
    else
        return getSimpleBookFilterQuery();
}

SearchFilter *BookSearchFilter::getQuranFilterQuery()
{
    QList<int> sowar = getSelectedSowar();
    SearchFilter *filter = new SearchFilter();
    filter->clause = BooleanClause::MUST;

    BooleanQuery *q = new BooleanQuery();
    q->setMaxClauseCount(0x7FFFFFFFL);

    BooleanQuery *soraQuery = new BooleanQuery();
    soraQuery->setMaxClauseCount(0x7FFFFFFFL);

    foreach(int soraNumber, sowar) {
        wchar_t *idStr = Utils::CLucene::intToWChar(soraNumber);
        Term *term = new Term(QURAN_SORA_FIELD, idStr);
        TermQuery *termQuery = new TermQuery(term);

        soraQuery->add(termQuery, BooleanClause::SHOULD);
    }

    wchar_t *bookIdStr = Utils::CLucene::intToWChar(m_book->id);
    Term *term = new Term(BOOK_ID_FIELD, bookIdStr);
    TermQuery *bookQuery = new TermQuery(term);

    q->add(bookQuery, BooleanClause::MUST);
    if(!sowar.isEmpty() && sowar.size() != 114)
        q->add(soraQuery, BooleanClause::MUST);

    filter->filterQuery = q;

    return filter;
}

SearchFilter *BookSearchFilter::getSimpleBookFilterQuery()
{
    QList<int> titles = getSelectedTitles();
    SearchFilter *filter = new SearchFilter();
    filter->clause = BooleanClause::MUST;

    BooleanQuery *q = new BooleanQuery();
    q->setMaxClauseCount(0x7FFFFFFFL);

    BooleanQuery *pagesQuery = new BooleanQuery();
    pagesQuery->setMaxClauseCount(0x7FFFFFFFL);

    foreach(int titleId, titles) {
        wchar_t *idStr = Utils::CLucene::intToWChar(titleId);
        Term *term = new Term(TITLE_ID_FIELD, idStr);
        TermQuery *termQuery = new TermQuery(term);

        pagesQuery->add(termQuery, BooleanClause::SHOULD);
    }

    wchar_t *bookIdStr = Utils::CLucene::intToWChar(m_book->id);
    Term *term = new Term(BOOK_ID_FIELD, bookIdStr);
    TermQuery *termQuery = new TermQuery(term);

    q->add(termQuery, BooleanClause::MUST);

    if(!titles.isEmpty())
        q->add(pagesQuery, BooleanClause::MUST);

    filter->filterQuery = q;

    return filter;
}

QList<int> BookSearchFilter::getSelectedSowar()
{
    QList<int> sowar;
    QModelIndex index = m_model->index(0, 0);

    while(index.isValid()) {
        if(index.data(Qt::CheckStateRole).toInt() ==  Qt::Checked) {
            sowar.append(index.data(ItemRole::soraRole).toInt());
        }

        index = index.sibling(index.row()+1, 0);
    }

    return sowar;
}

QList<int> BookSearchFilter::getSelectedTitles()
{
    QList<int> titles;
    QModelIndex index = m_model->index(0, 0);

    while(index.isValid()) {
        if(index.data(Qt::CheckStateRole).toInt() ==  Qt::Checked) {
            titles.append(index.data(ItemRole::idRole).toInt());
        }

        getChildTitles(index, titles);

        index = index.sibling(index.row()+1, 0);
    }

    return titles;
}

void BookSearchFilter::getChildTitles(const QModelIndex &index, QList<int> &titles)
{
    if(index.isValid()) {
        QModelIndex child = index.child(0, 0);
        while(child.isValid()) {
            if(child.data(Qt::CheckStateRole).toInt() ==  Qt::Checked) {
                titles.append(child.data(ItemRole::idRole).toInt());
            }

            getChildTitles(child, titles);

            child = index.child(child.row()+1, 0);
        }
    }
}

void BookSearchFilter::open()
{
    ml_return_on_fail2(m_book, "BookSearchFilter::open book is null");

    ml_return_on_fail2(QFile::exists(m_book->path),
               tr("لم يتم العثور على ملف الكتاب") << m_book->path);

    m_zip.setZipName(m_book->path);

    ml_return_on_fail2(m_zip.open(QuaZip::mdUnzip),
               tr("لا يمكن فتح ملف الكتاب") << m_book->path << "\nError:" << m_zip.getZipError());
}

void BookSearchFilter::close()
{
    m_zip.close();
}

void BookSearchFilter::loadQuranModel(QStandardItemModel *model)
{
    for(int i=1; i<=114; i++) {
        QuranSora *sora = MW->readerHelper()->getQuranSora(i);
        if(sora) {
            QStandardItem *soraItem = new QStandardItem();
            soraItem->setText(sora->name);
            soraItem->setData(i, ItemRole::soraRole);
            soraItem->setCheckable(true);

            model->appendRow(soraItem);
        }
    }
}

void BookSearchFilter::loadSimpleBookModel(QStandardItemModel *model)
{
    QuaZipFile titleFile(&m_zip);

    ml_return_on_fail2(m_zip.setCurrentFile("titles.xml"), "loadSimpleBookModel: setCurrentFile error"  << titleFile.getZipError());
    ml_return_on_fail2(titleFile.open(QIODevice::ReadOnly), "loadSimpleBookModel: open error" << titleFile.getZipError());

    QDomDocument doc = Utils::Xml::getDomDocument(&titleFile);
    QDomElement root = doc.documentElement();
    QDomElement element = root.firstChildElement();

    while(!element.isNull()) {
        readItem(element, model->invisibleRootItem());

        element = element.nextSiblingElement();
    }
}

void BookSearchFilter::readItem(QDomElement &element, QStandardItem *parent)
{
    QStandardItem *item = new QStandardItem();
    item->setText(element.firstChildElement("text").text());
    item->setData(element.attribute("pageID").toInt(), ItemRole::idRole);
    item->setCheckable(true);

    if(element.childNodes().count() > 1) {
        QDomElement child = element.firstChildElement("title");

        while(!child.isNull()) {
            readItem(child, item);

            child = child.nextSiblingElement("title");
        }
    }

    if(parent)
        parent->appendRow(item);
}
