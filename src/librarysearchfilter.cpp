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
    m_forceFilter = false;
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

void LibrarySearchFilter::selectItem(QStandardItem *item, const QList<int> &ids)
{
    ml_return_on_fail(item);

    item->setCheckState(ids.contains(item->data(ItemRole::idRole).toInt())
                        ? Qt::Checked : Qt::Unchecked);

    for(int i=0; i<item->rowCount(); i++) {
        selectItem(item->child(i), ids);
    }
}

SearchFilter *LibrarySearchFilter::getFilterQuery()
{
    generateLists();

    int count = 0;

    // Every thing is selected we don't need a filter
    if(!m_forceFilter && (!unSelecCount() || !selectedCount())) {
        return 0;
    }

    QList<int> books;
    SearchFilter *filter = new SearchFilter();
    BooleanQuery *q = new BooleanQuery();
    q->setMaxClauseCount(0x7FFFFFFFL);

    if(m_forceFilter && (!selectedCount() || !unSelecCount())) {
        books = selectedCount() ? selectedID() : unSelectedID();
        filter->clause = BooleanClause::MUST;
    }

    if(books.isEmpty()) {
        if(selectedCount() <= unSelecCount()) {
            books = selectedID();
            filter->clause = BooleanClause::MUST;
        } else {
            books = unSelectedID();
            filter->clause = BooleanClause::MUST_NOT;
        }
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

void LibrarySearchFilter::setSelectedItems(const QList<int> &ids)
{
    QStandardItem *item = m_model->invisibleRootItem();
    for(int i=0; i<item->rowCount(); i++) {
        selectItem(item->child(i), ids);
    }
}

QList<int> LibrarySearchFilter::getSelectedItems()
{
    generateLists();
    return m_selectedBooks;
}

void LibrarySearchFilter::setForceFilter(bool force)
{
    m_forceFilter = force;
}
