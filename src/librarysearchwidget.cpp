#include "librarysearchwidget.h"
#include "arabicanalyzer.h"
#include "clconstants.h"
#include "clheader.h"
#include "clutils.h"
#include "librarysearcher.h"
#include "librarysearchfilter.h"
#include "resultwidget.h"
#include "searchfiltermanager.h"
#include "sortfilterproxymodel.h"
#include "ui_searchwidget.h"
#include "utils.h"

#include <qmessagebox.h>
#include <qtreeview.h>

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
                           QList<int>() << 350);

    m_filterManager = filter;

    loadDefaultSearchField();
}

SearchFilter *LibrarySearchWidget::getSearchFilterQuery()
{
    return m_filterManager->getFilterQuery();
}
