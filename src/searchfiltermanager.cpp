#include "searchfiltermanager.h"
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

SearchFilterManager::SearchFilterManager(QObject *parent)
    : QObject(parent),
      m_model(0),
      m_treeView(0),
      m_role(Qt::DisplayRole),
      m_filterColumn(0),
      m_proccessItemChange(true)
{
    m_libraryInfo = MW->libraryInfo();
    m_filterModel = new SortFilterProxyModel(this);

    setupMenu();
    QTime time;
    time.start();
    loadModel();
    qDebug("Load search model take %d ms", time.elapsed());
}

SearchFilterManager::~SearchFilterManager()
{
    delete m_filterModel;
}

void SearchFilterManager::setTreeView(QTreeView *view)
{
    m_treeView = view;
}

void SearchFilterManager::setLineEdit(FancyLineEdit *edit)
{
    m_lineEdit = edit;
    m_lineEdit->setMenu(m_menu);

    connect(m_lineEdit, SIGNAL(textChanged(QString)),
            SLOT(setFilterText(QString)));
}

void SearchFilterManager::open()
{
    if(!m_indexDB.isOpen()) {
        m_indexDB = QSqlDatabase::addDatabase("QSQLITE", "SearchFilterManager_");
        m_indexDB.setDatabaseName(m_libraryInfo->booksIndexPath());

        if (!m_indexDB.open())
            LOG_DB_ERROR(m_indexDB);
    }
}

void SearchFilterManager::close()
{
    // TODO: remove this database properly
    QString conn = m_indexDB.connectionName();
    m_indexDB.close();
    m_indexDB = QSqlDatabase();

    QSqlDatabase::removeDatabase(conn);
}

void SearchFilterManager::setupMenu()
{
    m_menu = new QMenu(0);

    QAction *actionClearText = new QAction(tr("مسح النص"), this);
    m_menu->addAction(actionClearText);

    QAction *actionSelected = new QAction(tr("عرض ما تم اختياره"), this);
    m_menu->addAction(actionSelected);

    m_menu->addSeparator();

    QMenu *menu2 =  m_menu->addMenu(tr("بحث في"));
    m_actFilterByBooks = menu2->addAction(tr("اسماء الكتب"));
    m_actFilterByBetaka = menu2->addAction(tr("بطاقة الكتاب"));
    m_actFilterByAuthors = menu2->addAction(tr("اسماء المؤلفين"));

    m_actFilterByBooks->setCheckable(true);
    m_actFilterByBooks->setChecked(true);
    m_actFilterByBetaka->setCheckable(true);
    m_actFilterByAuthors->setCheckable(true);

    m_filterColumn = 0;
    m_role = Qt::DisplayRole;

    connect(m_actFilterByBooks, SIGNAL(triggered()), SLOT(changeFilterAction()));
    connect(m_actFilterByBetaka, SIGNAL(triggered()), SLOT(changeFilterAction()));
    connect(m_actFilterByAuthors, SIGNAL(triggered()), SLOT(changeFilterAction()));
    connect(actionSelected, SIGNAL(triggered()), SLOT(showSelected()));
    connect(actionClearText, SIGNAL(triggered()), SLOT(clearFilter()));
}

void SearchFilterManager::loadModel()
{
    if(m_model)
        delete m_model;

    m_model = new QStandardItemModel();
    m_model->setHorizontalHeaderLabels(QStringList()
                                       << tr("الكتاب")
                                       << tr("المؤلف"));


    open();

    QStandardItem *catIem = new QStandardItem();
    catIem->setText(tr("[غير مصنف]"));
    catIem->setData(ItemType::CategorieItem, ItemRole::typeRole);;
    catIem->setCheckable(true);

    getBookItems(0, catIem);

    if(catIem->rowCount())
        m_model->appendRow(catIem);

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

            m_model->appendRow(catIem);
        }
    } else {
        LOG_SQL_ERROR(query);
    }

    m_filterModel->setSourceModel(m_model);
    connect(m_model, SIGNAL(itemChanged(QStandardItem*)),
            SLOT(itemChanged(QStandardItem*)));

    close();
}

void SearchFilterManager::getBookItems(int catID, QStandardItem *catItem)
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

SortFilterProxyModel *SearchFilterManager::filterModel()
{
    return m_filterModel;
}

void SearchFilterManager::setFilterText(QString text)
{
    m_filterText = text;

    text.replace(QRegExp("[\\x0627\\x0622\\x0623\\x0625]"), "[\\x0627\\x0622\\x0623\\x0625]");//ALEFs
    text.replace(QRegExp("[\\x0647\\x0629]"), "[\\x0647\\x0629]"); //TAH_MARBUTA, HEH
    text.replace(QRegExp("[\\x064A\\x0649]"), "[\\x064A\\x0649]"); //YAH, ALEF MAKSOURA

    m_filterModel->setFilterKeyColumn(m_filterColumn);
    m_filterModel->setFilterRole(m_role);
    m_filterModel->setFilterRegExp(text);

    enableCatSelection();
}

void SearchFilterManager::changeFilterAction()
{
    QAction *act = qobject_cast<QAction*>(sender());
    QList<QAction*> actList;

    if(act) {
        if(!act->isChecked()) {
            act->setChecked(true);
            return;
        }

        actList << m_actFilterByBooks;
        actList << m_actFilterByBetaka;
        actList << m_actFilterByAuthors;

        foreach (QAction *a, actList) {
            if(act != a)
                a->setChecked(false);
        }

        if(act == m_actFilterByBooks) {
            m_filterColumn = 0;
            m_role = Qt::DisplayRole;
        } else if(act == m_actFilterByAuthors) {
            m_filterColumn = 1;
            m_role = Qt::DisplayRole;
        } else if(act == m_actFilterByBetaka) {
            m_filterColumn = 0;
            m_role = Qt::ToolTipRole;
        }

        m_filterModel->setFilterKeyColumn(m_filterColumn);
        m_filterModel->setFilterRole(m_role);
    }
}

void SearchFilterManager::showSelected()
{
    QVariant checked(Qt::Checked);

    m_filterModel->setFilterKeyColumn(0);
    m_filterModel->setFilterRole(Qt::CheckStateRole);
    m_filterModel->setFilterFixedString(checked.toString());
}

void SearchFilterManager::showUnSelected()
{
    QVariant unChecked(Qt::Unchecked);

    m_filterModel->setFilterRole(Qt::CheckStateRole);
    m_filterModel->setFilterFixedString(unChecked.toString());
}

void SearchFilterManager::clearFilter()
{
    m_lineEdit->clear();
}

void SearchFilterManager::enableCatSelection()
{
    bool catCheckable = m_filterModel->filterRegExp().isEmpty();

    int rowCount = m_filterModel->rowCount();
    QModelIndex topLeft = m_filterModel->index(0, 0);
    QModelIndex bottomRight = m_filterModel->index(rowCount-1, 0);
    QItemSelection selection(topLeft, bottomRight);
    QItemSelection sourceSelection = m_filterModel->mapSelectionToSource(selection);

    foreach (QModelIndex index, sourceSelection.indexes()) {
        QStandardItem *item = m_model->itemFromIndex(index);
        if(item) {
            item->setCheckable(catCheckable);
            setCatCheckable(item, catCheckable);
        }
    }
}

void SearchFilterManager::setCatCheckable(QStandardItem *parent, bool checkable)
{
    int row = 0;
    QStandardItem *child = parent->child(row);

    while(child) {
        if(child->rowCount()) {
            child->setCheckable(checkable);

            setCatCheckable(child, checkable);
        }

        child = parent->child(++row);
    }
}

void SearchFilterManager::selectAllBooks()
{
    int rowCount = m_model->rowCount();
    QModelIndex topLeft = m_model->index(0, 0);
    QModelIndex bottomRight = m_model->index(rowCount-1, 0);
    QItemSelection selection(topLeft, bottomRight);

    foreach (QModelIndex index, selection.indexes()) {
        m_model->setData(index, Qt::Checked, Qt::CheckStateRole);
    }
}

void SearchFilterManager::unSelectAllBooks()
{
    int rowCount = m_model->rowCount();
    QModelIndex topLeft = m_model->index(0, 0);
    QModelIndex bottomRight = m_model->index(rowCount-1, 0);
    QItemSelection selection(topLeft, bottomRight);

    foreach (QModelIndex index, selection.indexes()) {
        m_model->setData(index, Qt::Unchecked, Qt::CheckStateRole);
    }
}

void SearchFilterManager::selectVisibleBooks()
{
    int rowCount = m_filterModel->rowCount();
    QModelIndex topLeft = m_filterModel->index(0, 0);
    QModelIndex bottomRight = m_filterModel->index(rowCount-1, 0);
    QItemSelection selection(topLeft, bottomRight);

    foreach (QModelIndex index, selection.indexes()) {
        if(hasChilds(index)) {
            checkIndex(m_filterModel, index, Qt::Checked);
        } else {
            m_filterModel->setData(index, Qt::Checked, Qt::CheckStateRole);
        }
    }
}

void SearchFilterManager::unSelectVisibleBooks()
{
    int rowCount = m_filterModel->rowCount();
    QModelIndex topLeft = m_filterModel->index(0, 0);
    QModelIndex bottomRight = m_filterModel->index(rowCount-1, 0);
    QItemSelection selection(topLeft, bottomRight);

    foreach (QModelIndex index, selection.indexes()) {
        if(hasChilds(index)) {
            checkIndex(m_filterModel, index, Qt::Unchecked);
        } else {
            m_filterModel->setData(index, Qt::Unchecked, Qt::CheckStateRole);
        }
    }
}

void SearchFilterManager::checkIndex(QAbstractItemModel *model, const QModelIndex &parent, Qt::CheckState checkStat)
{
    QModelIndex child = parent.child(0, 0);

    while(child.isValid()) {
        if(hasChilds(child))
            checkIndex(model, child, checkStat);
        else
            model->setData(child, checkStat, Qt::CheckStateRole);

        child = parent.child(child.row()+1, 0);
    }
}

bool SearchFilterManager::hasChilds(const QModelIndex &index)
{
    return index.child(0, 0).isValid();
}

void SearchFilterManager::expandFilterView()
{
    Q_ASSERT(m_treeView);

    m_treeView->expandAll();
}

void SearchFilterManager::collapseFilterView()
{
    Q_ASSERT(m_treeView);

    m_treeView->collapseAll();
}

void SearchFilterManager::itemChanged(QStandardItem *item)
{
    if(m_proccessItemChange){
        m_proccessItemChange = false;

        if(item->checkState() != Qt::PartiallyChecked) {
            checkChilds(item, item->checkState());
        }

        QStandardItem *parent = item->parent();

        while(parent) {
            int checkItems = 0;
            int partiallyChecked = 0;

            for(int i=0; i<parent->rowCount(); i++) {
                if(parent->child(i)->checkState()==Qt::Checked)
                    checkItems++;
                else if(parent->child(i)->checkState()==Qt::PartiallyChecked)
                    partiallyChecked++;
            }

            if(checkItems == 0 && partiallyChecked == 0)
                parent->setCheckState(Qt::Unchecked);
            else if(checkItems < parent->rowCount() || partiallyChecked)
                parent->setCheckState(Qt::PartiallyChecked);
            else
                parent->setCheckState(Qt::Checked);

            parent = parent->parent();
        }

        m_proccessItemChange = true;
    }
}

void SearchFilterManager::checkChilds(QStandardItem *parent, Qt::CheckState checkStat)
{
    int row = 0;
    QStandardItem *child = parent->child(0);

    while(child) {
        child->setCheckState(checkStat);
        checkChilds(child, checkStat);

        child = parent->child(++row);
    }
}

void SearchFilterManager::generateLists()
{
    m_selectedBooks.clear();
    m_unSelectedBooks.clear();

    QModelIndex index = m_model->index(0, 0);
    while(index.isValid()) {
        getBooks(index, ItemType::BookItem);

        index = index.sibling(index.row()+1, 0);
    }
}

void SearchFilterManager::getBooks(const QModelIndex &index, int role)
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

QList<int> SearchFilterManager::selectedBooks()
{
    return m_selectedBooks;
}

QList<int> SearchFilterManager::unSelectedBooks()
{
    return m_unSelectedBooks;
}

int SearchFilterManager::selectedBooksCount()
{
    return m_selectedBooks.count();
}

int SearchFilterManager::unSelectBooksCount()
{
    return m_unSelectedBooks.count();
}

SearchFilter *SearchFilterManager::getFilterQuery()
{
    generateLists();

    SearchFilter *filter = new SearchFilter();
    int count = 0;

    // Every thing is selected we don't need a filter
    if(unSelectBooksCount()==0 || selectedBooksCount()==0 ) {
        filter->filterQuery = 0;
        return filter;
    }

    QList<int> books;
    BooleanQuery *q = new BooleanQuery();
    q->setMaxClauseCount(0x7FFFFFFFL);

    if(selectedBooksCount() <= unSelectBooksCount()) {
        books = selectedBooks();
        filter->clause = BooleanClause::MUST;
    } else {
        books = unSelectedBooks();
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
