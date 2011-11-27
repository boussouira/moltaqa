#include "searchview.h"
#include "tabwidget.h"
#include "librarysearchwidget.h"
#include "booksearchwidget.h"
#include "mainwindow.h"
#include "indexmanager.h"

#include <qboxlayout.h>
#include <qmessagebox.h>

SearchView::SearchView(QWidget *parent) : AbstarctView(parent)
{
    m_layout = new QVBoxLayout(this);
    m_tabWidget = new TabWidget(this);
    m_tabWidget->setCanMoveToOtherTabWidget(false);

    m_layout->addWidget(m_tabWidget);
    m_layout->setContentsMargins(0, 6, 0, 0);

    QToolBar *bar = new QToolBar(this);

    bar->addAction(QIcon(":/menu/images/tab-new.png"),
                   tr("تبويب بحث جديد"),
                   this, SLOT(openNewTab()));

    bar->addAction(QIcon(":/menu/images/switch.png"),
                   tr("تنقل بين نافذة البحث والنتائج"),
                   this, SLOT(switchSearchWidget()));

    m_toolBars << bar;
    setLayout(m_layout);

    connect(m_tabWidget, SIGNAL(tabCloseRequested(int)), SLOT(closeTab(int)));
    connect(this, SIGNAL(lastTabClosed()), SIGNAL(hideMe()));
}

QString SearchView::title()
{
    return tr("نافذة البحث");
}

bool SearchView::ensureTabIsOpen()
{
    if(MW->indexManager()->isIndexing()) {
        QMessageBox::warning(this,
                             tr("بحث متقدم"),
                             tr("البرنامج يقوم بفهرسة الكتب" "\n"
                                "من فضلك انتظر حتى تنتهي الفهرسة"));
        return false;
    }

    if(m_tabWidget->count() <= 0)
        newTab(SearchWidget::LibrarySearch);
    else
        emit showMe();

    return true;
}

void SearchView::newTab(SearchWidget::SearchType searchType, int bookID)
{
    SearchWidget *searchWidget = 0;
    if(searchType == SearchWidget::LibrarySearch) {
        searchWidget = new LibrarySearchWidget(this);
    } else {
        qCritical("SearchView: Unknow search type: %d", searchType);
        return;
    }

    searchWidget->init();
    searchWidget->setAutoFillBackground(true);

    int tabIndex = m_tabWidget->addTab(searchWidget,
                                       QIcon(":/menu/images/find.png"),
                                       tr("بحث"));

    m_tabWidget->setCurrentIndex(tabIndex);

    emit showMe();
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
    SearchWidget *w = qobject_cast<LibrarySearchWidget*>(m_tabWidget->currentWidget());

    if(w) {
        w->toggleWidget();
    }
}

void SearchView::openNewTab()
{
    newTab(SearchWidget::LibrarySearch);
}
