#include "searchview.h"
#include "tabwidget.h"
#include "searchwidget.h"
#include <qboxlayout.h>

SearchView::SearchView(QWidget *parent) : AbstarctView(parent)
{
    m_layout = new QVBoxLayout(this);
    m_tabWidget = new TabWidget(this);
//    m_tabWidget->setDocumentMode(true);

    m_layout->addWidget(m_tabWidget);
    m_layout->setContentsMargins(0, 6, 0, 0);

    QToolBar *bar = new QToolBar(this);
    bar->addAction(QIcon(":/menu/images/tab-new.png"), tr("تبويب بحث جديد"), this, SLOT(newTab()));
    bar->addAction(QIcon(":/menu/images/switch.png"), tr("تنقل بين نافذة البحث والنتائج"), this, SLOT(switchSearchWidget()));

    m_toolBars << bar;
    setLayout(m_layout);

    connect(m_tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
    connect(this, SIGNAL(lastTabClosed()), SIGNAL(hideMe()));
}

QString SearchView::title()
{
    return tr("نافذة البحث");
}

void SearchView::ensureTabIsOpen()
{
    if(m_tabWidget->count() <= 0)
        newTab();
}

void SearchView::newTab()
{
    SearchWidget *searchWidget = new SearchWidget(this);
    searchWidget->setAutoFillBackground(true);

    int tabIndex = m_tabWidget->addTab(searchWidget,
                                       QIcon(":/menu/images/find.png"),
                                       tr("بحث"));

    m_tabWidget->setCurrentIndex(tabIndex);
}

void SearchView::closeTab(int index)
{
    QWidget *w = m_tabWidget->widget(index);
    m_tabWidget->removeTab(index);

    delete w;

    if(m_tabWidget->count() <= 0)
        emit lastTabClosed();
}

void SearchView::switchSearchWidget()
{
    SearchWidget *w = qobject_cast<SearchWidget*>(m_tabWidget->currentWidget());

    if(w) {
        w->toggleWidget();
    }
}
