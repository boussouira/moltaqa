#include "booksearchwidget.h"
#include "ui_searchwidget.h"
#include "resultwidget.h"
#include "mainwindow.h"
#include "booksearchfilter.h"
#include "librarybookmanager.h"

BookSearchWidget::BookSearchWidget(QWidget *parent) :
    SearchWidget(parent)
{
}

BookSearchWidget::~BookSearchWidget()
{
}

void BookSearchWidget::init(int bookID)
{
    m_bookInfo = LibraryBookManager::instance()->getLibraryBook(bookID);

    if(!m_bookInfo)
        return;

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
    QString m_connectionName = QString("book_i%1_").arg(m_bookInfo->bookID);
    while(QSqlDatabase::contains(m_connectionName))
        m_connectionName.append("_");

    m_bookDB = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    m_bookDB.setDatabaseName(m_bookInfo->bookPath);

    if (!m_bookDB.open()) {
        LOG_DB_ERROR(m_bookDB);
        return;
    }
    m_bookQuery = QSqlQuery(m_bookDB);

}
