#include "librarysearchwidget.h"
#include "ui_searchwidget.h"
#include "resultwidget.h"
#include "clheader.h"
#include "clutils.h"
#include "utils.h"
#include "clconstants.h"
#include "arabicanalyzer.h"
#include "searchfiltermanager.h"
#include "librarysearchfilter.h"
#include "librarysearcher.h"
#include <qmessagebox.h>

LibrarySearchWidget::LibrarySearchWidget(QWidget *parent) :
    SearchWidget(parent)
{
}

LibrarySearchWidget::~LibrarySearchWidget()
{
    Utils::Widget::save(ui->treeView, "LibrarySearch", 2);
}

void LibrarySearchWidget::init(int bookID)
{
    Q_UNUSED(bookID);

    LibrarySearchFilter *filter = new LibrarySearchFilter(this);
    filter->setTreeView(ui->treeView);
    filter->setLineEdit(ui->lineFilter);
    filter->loadModel();

    m_resultWidget = new ResultWidget(this);
    ui->stackedWidget->insertWidget(1, m_resultWidget);

    ui->treeView->setModel(filter->filterModel());
    ui->treeView->setColumnWidth(0, 300);

    Utils::Widget::restore(ui->treeView, "LibrarySearch",
                           QList<int>() << 350 << 200);

    m_filterManager = filter;
}

SearchFilter *LibrarySearchWidget::getSearchFilterQuery()
{
    return m_filterManager->getFilterQuery();
}
