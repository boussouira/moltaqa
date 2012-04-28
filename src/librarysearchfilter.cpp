#include "librarysearchfilter.h"
#include "mainwindow.h"
#include "utils.h"
#include "modelenums.h"
#include "modelutils.h"
#include "clheader.h"
#include "clconstants.h"
#include "clutils.h"
#include "libraryenums.h"
#include "librarymanager.h"
#include "booklistmanager.h"
#include "clucenequery.h"

#include <qsqlquery.h>
#include <qitemselectionmodel.h>
#include <qdatetime.h>

LibrarySearchFilter::LibrarySearchFilter(QObject *parent) :
    SearchFilterManager(parent)
{
}

void LibrarySearchFilter::loadModel()
{
    BookListManager *bookListManager = LibraryManager::instance()->bookListManager();
    QStandardItemModel *model = Utils::Model::cloneModel(bookListManager->bookListModel());
    Utils::Model::setModelCheckable(model);

    setSourceModel(model);
}

void LibrarySearchFilter::generateLists()
{
    m_selectedBooks.clear();
    m_unSelectedBooks.clear();

    QModelIndex index = m_model->index(0, 0);
    while(index.isValid()) {
        addBook(index);
        getBooks(index);

        index = index.sibling(index.row()+1, 0);
    }
}

void LibrarySearchFilter::getBooks(const QModelIndex &index)
{
    QModelIndex child = index.child(0, 0);
    while(child.isValid()) {
        addBook(child);
        getBooks(child);

        child = index.child(child.row()+1, 0);
    }
}

void LibrarySearchFilter::addBook(const QModelIndex &index)
{
    if(index.data(ItemRole::itemTypeRole).toInt() == ItemType::BookItem) {
        if(index.data(Qt::CheckStateRole).toInt() ==  Qt::Checked) {
            m_selectedBooks.append(index.data(ItemRole::idRole).toInt());
        } else {
            m_unSelectedBooks.append(index.data(ItemRole::idRole).toInt());
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

    int count = 0;

    // Every thing is selected we don't need a filter
    if(unSelecCount()==0 || selectedCount()==0 ) {
        return 0;
    }

    QList<int> books;
    SearchFilter *filter = new SearchFilter();
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
        wchar_t *idStr = Utils::CLucene::intToWChar(id);
        Term *term = new Term(BOOK_ID_FIELD, idStr);
        TermQuery *termQuery = new TermQuery(term);

        q->add(termQuery, BooleanClause::SHOULD);
        count++;
    }

    ml_warn_on_fail(count, "LibrarySearchFilter::getFilterQuery count is 0");

    filter->query = q;
    filter->selected = selectedCount();
    filter->unSelected = unSelecCount();

    return filter;
}
