#include "booksearchfilter.h"
#include "mainwindow.h"
#include "utils.h"
#include "modelenums.h"
#include "clheader.h"
#include "clconstants.h"
#include "clutils.h"
#include "libraryenums.h"

#include <qsqlquery.h>
#include <qitemselectionmodel.h>
#include <qdatetime.h>

BookSearchFilter::BookSearchFilter(QObject *parent) :
    SearchFilterManager(parent)
{
    setChangeFilterColumn(false);
    setAutoSelectParent(false);
}

BookSearchFilter::~BookSearchFilter()
{
}

void BookSearchFilter::setLibraryBook(LibraryBook *book)
{
    m_book = book;
}

void BookSearchFilter::loadModel()
{
    if(m_model)
        delete m_model;

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
        wchar_t *idStr = Utils::intToWChar(soraNumber);
        Term *term = new Term(QURAN_SORA_FIELD, idStr);
        TermQuery *termQuery = new TermQuery(term);

        soraQuery->add(termQuery, BooleanClause::SHOULD);
    }

    wchar_t *bookIdStr = Utils::intToWChar(m_book->bookID);
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

    qDebug("Selected books: %d", titles.size());

    BooleanQuery *q = new BooleanQuery();
    q->setMaxClauseCount(0x7FFFFFFFL);

    BooleanQuery *pagesQuery = new BooleanQuery();
    pagesQuery->setMaxClauseCount(0x7FFFFFFFL);

    foreach(int titleId, titles) {
        wchar_t *idStr = Utils::intToWChar(titleId);
        Term *term = new Term(TITLE_ID_FIELD, idStr);
        TermQuery *termQuery = new TermQuery(term);

        pagesQuery->add(termQuery, BooleanClause::SHOULD);
    }

    wchar_t *bookIdStr = Utils::intToWChar(m_book->bookID);
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
    if(!m_indexDB.isOpen()) {
        m_indexDB = QSqlDatabase::addDatabase("QSQLITE", "BookSearchFilter_");
        m_indexDB.setDatabaseName(m_book->bookPath);

        if (!m_indexDB.open())
            LOG_DB_ERROR(m_indexDB);
    }
}

void BookSearchFilter::close()
{
    // TODO: remove this database properly
    QString conn = m_indexDB.connectionName();
    m_indexDB.close();
    m_indexDB = QSqlDatabase();

    QSqlDatabase::removeDatabase(conn);
}

void BookSearchFilter::loadQuranModel(QStandardItemModel *model)
{
    QSqlQuery query(m_indexDB);

    if(query.exec("SELECT id, soraName FROM quranSowar ORDER BY id")) {
        while(query.next()) {
            QStandardItem *soraItem = new QStandardItem();
            soraItem->setText(query.value(1).toString());
            soraItem->setData(query.value(0).toInt(), ItemRole::soraRole);
            soraItem->setCheckable(true);

            model->appendRow(soraItem);
        }
    }
}

void BookSearchFilter::loadSimpleBookModel(QStandardItemModel *model)
{
    childTitles(model, 0, 0);
}

void BookSearchFilter::childTitles(QStandardItemModel *model, QStandardItem *parentItem, int tid)
{
    QSqlQuery query(m_indexDB);
    query.exec(QString("SELECT id, parentID, pageID, title FROM bookIndex "
                          "WHERE parentID = %1 ORDER BY id").arg(tid));

    while(query.next()){
        QStandardItem *titleItem = new QStandardItem();
        titleItem->setText(query.value(3).toString());
        titleItem->setData(query.value(0).toInt(), ItemRole::idRole);
        titleItem->setCheckable(true);

        childTitles(model, titleItem, query.value(0).toInt());

        if(parentItem)
            parentItem->appendRow(titleItem);
        else
            model->appendRow(titleItem);
    }
}
