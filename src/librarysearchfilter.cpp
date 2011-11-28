#include "librarysearchfilter.h"
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

LibrarySearchFilter::LibrarySearchFilter(QObject *parent) :
    SearchFilterManager(parent)
{
}

void LibrarySearchFilter::loadModel()
{
    QStandardItemModel *model = new QStandardItemModel();
    model->setHorizontalHeaderLabels(QStringList()
                                       << tr("الكتاب")
                                       << tr("المؤلف"));


    open();

    QStandardItem *catIem = new QStandardItem();
    catIem->setText(tr("[غير مصنف]"));
    catIem->setData(ItemType::CategorieItem, ItemRole::typeRole);;
    catIem->setCheckable(true);

    getBookItems(0, catIem);

    if(catIem->rowCount())
        model->appendRow(catIem);

    QSqlQuery query(m_indexDB);
    query.prepare("SELECT id, title, description FROM catList ORDER BY id");
    if(query.exec()) {
        while(query.next()) {
            QStandardItem *catIem = new QStandardItem();
            catIem->setText(query.value(1).toString());
            catIem->setToolTip(query.value(2).toString());
            catIem->setData(ItemType::CategorieItem, ItemRole::typeRole);;
            catIem->setCheckable(true);

            getBookItems(query.value(0).toInt(), catIem);

            model->appendRow(catIem);
        }
    } else {
        LOG_SQL_ERROR(query);
    }

    setSourceModel(model);

    close();
}

void LibrarySearchFilter::open()
{
    if(!m_indexDB.isOpen()) {
        m_indexDB = QSqlDatabase::addDatabase("QSQLITE", "LibrarySearchFilter_");
        m_indexDB.setDatabaseName(m_libraryInfo->booksIndexPath());

        if (!m_indexDB.open())
            LOG_DB_ERROR(m_indexDB);
    }
}

void LibrarySearchFilter::close()
{
    // TODO: remove this database properly
    QString conn = m_indexDB.connectionName();
    m_indexDB.close();
    m_indexDB = QSqlDatabase();

    QSqlDatabase::removeDatabase(conn);
}

void LibrarySearchFilter::getBookItems(int catID, QStandardItem *catItem)
{
    QSqlQuery query(m_indexDB);

    query.prepare("SELECT booksList.id, booksList.bookDisplayName, "
                  "booksList.bookInfo, authorsList.name "
                  "FROM booksList LEFT JOIN authorsList "
                  "ON authorsList.id = booksList.authorID "
                  "WHERE booksList.bookCat = ? AND booksList.indexFlags = ?");

    query.bindValue(0, catID);
    query.bindValue(1, Enums::Indexed);

    if(query.exec()) {
        while(query.next()) {
            QStandardItem *bookItem = new QStandardItem();
            bookItem->setText(query.value(1).toString());
            bookItem->setToolTip(query.value(2).toString());
            bookItem->setData(ItemType::BookItem, ItemRole::typeRole);
            bookItem->setData(query.value(0).toInt(), ItemRole::idRole);
            bookItem->setCheckable(true);

            QStandardItem *authItem = new QStandardItem();
            authItem->setText(query.value(3).toString());

            QList<QStandardItem *> items;
            items << bookItem;
            items << authItem;

            catItem->appendRow(items);
        }
    } else {
        LOG_SQL_ERROR(query);
    }
}

void LibrarySearchFilter::generateLists()
{
    m_selectedBooks.clear();
    m_unSelectedBooks.clear();

    QModelIndex index = m_model->index(0, 0);
    while(index.isValid()) {
        getBooks(index, ItemType::BookItem);

        index = index.sibling(index.row()+1, 0);
    }
}

void LibrarySearchFilter::getBooks(const QModelIndex &index, int role)
{
    if(index.isValid()) {
        QModelIndex child = index.child(0, 0);
        while(child.isValid()) {
            if(child.data(ItemRole::typeRole).toInt() == role) {
                if(child.data(Qt::CheckStateRole).toInt() ==  Qt::Checked) {
                    m_selectedBooks.append(child.data(ItemRole::idRole).toInt());
                } else {
                    m_unSelectedBooks.append(child.data(ItemRole::idRole).toInt());
                }
            } else {
                getBooks(child, role);
            }

            child = index.child(child.row()+1, 0);
        }
    }
}

QList<int> LibrarySearchFilter::selectedID()
{
    return m_selectedBooks;
}

QList<int> LibrarySearchFilter::unSelectedID()
{
    return m_unSelectedBooks;
}

int LibrarySearchFilter::selectedCount()
{
    return m_selectedBooks.count();
}

int LibrarySearchFilter::unSelecCount()
{
    return m_unSelectedBooks.count();
}

SearchFilter *LibrarySearchFilter::getFilterQuery()
{
    generateLists();

    SearchFilter *filter = new SearchFilter();
    int count = 0;

    // Every thing is selected we don't need a filter
    if(unSelecCount()==0 || selectedCount()==0 ) {
        filter->filterQuery = 0;
        return filter;
    }

    QList<int> books;
    BooleanQuery *q = new BooleanQuery();
    q->setMaxClauseCount(0x7FFFFFFFL);

    if(selectedCount() <= unSelecCount()) {
        books = selectedID();
        filter->clause = BooleanClause::MUST;
    } else {
        books = unSelectedID();
        filter->clause = BooleanClause::MUST_NOT;
    }

    foreach(int id, books) {
        wchar_t *idStr = Utils::intToWChar(id);
        Term *term = new Term(BOOK_ID_FIELD, idStr);
        TermQuery *termQuery = new TermQuery(term);

        q->add(termQuery, BooleanClause::SHOULD);
        count++;
    }

    filter->filterQuery = count ? q : 0;

    return filter;
}
