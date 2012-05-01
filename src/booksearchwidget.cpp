#include "booksearchwidget.h"
#include "ui_searchwidget.h"
#include "resultwidget.h"
#include "mainwindow.h"
#include "booksearchfilter.h"
#include "librarybookmanager.h"

BookSearchWidget::BookSearchWidget(QWidget *parent) :
    SearchWidget(parent)
{
    m_bookManager = LibraryManager::instance()->bookManager();
    ui->labelSearchField->hide();
}

BookSearchWidget::~BookSearchWidget()
{
}

void BookSearchWidget::init(int bookID)
{
    m_bookInfo = m_bookManager->getLibraryBook(bookID);

    ml_return_on_fail(m_bookInfo);

    BookSearchFilter *filter= new BookSearchFilter(this);
    filter->setLibraryBook(m_bookInfo);
    filter->setTreeView(ui->treeView);
    filter->setLineEdit(ui->lineFilter);
    filter->loadModel();

    m_resultWidget = new ResultWidget(this);
    ui->stackedWidget->insertWidget(1, m_resultWidget);

    ui->treeView->setSortingEnabled(false);
    ui->treeView->setModel(filter->filterModel());

    m_filterManager = filter;
}

SearchFilter *BookSearchWidget::getSearchFilterQuery()
{
    return m_filterManager->getFilterQuery();
}

void BookSearchWidget::loadModel()
{
    QString m_connectionName = QString("book_i%1_").arg(m_bookInfo->id);
    while(QSqlDatabase::contains(m_connectionName))
        m_connectionName.append("_");

    m_bookDB = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    m_bookDB.setDatabaseName(m_bookInfo->path);

    if (!m_bookDB.open()) {
        ml_warn_db_error(m_bookDB);
        return;
    }
    m_bookQuery = QSqlQuery(m_bookDB);

}
