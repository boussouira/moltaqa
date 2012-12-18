#include "favouritessearchwidget.h"
#include "ui_searchwidget.h"
#include "favouritessearchfilter.h"
#include "utils.h"
#include "resultwidget.h"

FavouritesSearchWidget::FavouritesSearchWidget(QWidget *parent) :
    SearchWidget(parent)
{
}

FavouritesSearchWidget::~FavouritesSearchWidget()
{
}

void FavouritesSearchWidget::init(int bookID)
{
    Q_UNUSED(bookID);

    FavouritesSearchFilter *filter = new FavouritesSearchFilter(this);
    filter->setTreeView(ui->treeView);
    filter->setLineEdit(ui->lineFilter);
    filter->loadModel();

    m_resultWidget = new ResultWidget(this);
    ui->stackedWidget->insertWidget(1, m_resultWidget);

    ui->treeView->setModel(filter->filterModel());
    ui->treeView->setColumnWidth(0, 300);

    Utils::Widget::restore(ui->treeView, "FavouritesSearch",
                           QList<int>() << 350);

    m_filterManager = filter;

    loadDefaultSearchField();
}

SearchFilter *FavouritesSearchWidget::getSearchFilterQuery()
{
    return m_filterManager->getFilterQuery();
}
