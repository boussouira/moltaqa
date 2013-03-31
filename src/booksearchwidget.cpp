#include "booksearchwidget.h"
#include "ui_searchwidget.h"
#include "resultwidget.h"
#include "mainwindow.h"
#include "booksearchfilter.h"
#include "librarybookmanager.h"
#include "librarymanager.h"
#include "sortfilterproxymodel.h"

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

    ui->widgetResultSort->setVisible(!m_bookInfo->isQuran());
    ui->widgetSearchField->setVisible(!m_bookInfo->isQuran());
    ui->checkShowPageInfo->setVisible(!m_bookInfo->isQuran());
    ui->checkShowResultTitles->setVisible(!m_bookInfo->isQuran());

    m_filterManager = filter;
}

SearchFilter *BookSearchWidget::getSearchFilterQuery()
{
    return m_filterManager->getFilterQuery();
}
