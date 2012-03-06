#include "searchview.h"
#include "tabwidget.h"
#include "librarysearchwidget.h"
#include "booksearchwidget.h"
#include "mainwindow.h"
#include "indexmanager.h"
#include "librarybookmanager.h"

#include <qboxlayout.h>
#include <qmessagebox.h>
#include <qaction.h>

SearchView::SearchView(QWidget *parent) : AbstarctView(parent)
{
    m_layout = new QVBoxLayout(this);
    m_tabWidget = new TabWidget(this);
    m_tabWidget->setCanMoveToOtherTabWidget(false);

    m_layout->addWidget(m_tabWidget);
    m_layout->setContentsMargins(0, 6, 0, 0);

    QToolBar *bar = new QToolBar(this);

    QAction *actNewTab = new QAction(QIcon(":/images/tab-new.png"),
                               tr("تبويب بحث جديد"), this);
    QAction *actSwitchTab = new QAction(QIcon(":/images/switch.png"),
                                     tr("تنقل بين نافذة البحث والنتائج"), this);
    bar->addAction(actNewTab);
    bar->addAction(actSwitchTab);

    m_toolBars << bar;
    setLayout(m_layout);

    connect(actNewTab, SIGNAL(triggered()), SLOT(openNewTab()));
    connect(actSwitchTab, SIGNAL(triggered()), SLOT(switchSearchWidget()));
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
        newTab(SearchWidget::LibrarySearch);
    else
        emit showMe();
}

bool SearchView::canSearch(bool showMessage)
{
    if(MW->indexManager()->isIndexing()) {
        if(showMessage) {
            QMessageBox::warning(this,
                                 tr("بحث متقدم"),
                                 tr("البرنامج يقوم بفهرسة الكتب" "\n"
                                    "من فضلك انتظر حتى تنتهي الفهرسة"));
        }

        return false;
    }

    return true;
}

void SearchView::newTab(SearchWidget::SearchType searchType, int bookID)
{
    if(!canSearch())
        return;

    SearchWidget *searchWidget = 0;
    if(searchType == SearchWidget::LibrarySearch) {
        searchWidget = new LibrarySearchWidget(this);
    } else if(searchType == SearchWidget::BookSearch) {
        searchWidget = new BookSearchWidget(this);
    } else {
        qCritical("SearchView: Unknow search type: %d", searchType);
        return;
    }

    searchWidget->init(bookID);
    searchWidget->setAutoFillBackground(true);

    QString tabLabel = tr("بحث في المكتبة");
    QString tabTooltip = tabLabel;

    if(searchType == SearchWidget::BookSearch) {
        LibraryBook *book = LibraryBookManager::instance()->getLibraryBook(bookID);
        if(book) {
            QString bookName = book->bookDisplayName;
            QString shortBookName = Utils::abbreviate(bookName, 20);

            tabLabel = tr("بحث في %1").arg(shortBookName);
            tabTooltip = tr("بحث في %1").arg(bookName);
        }
    }

    int tabIndex = m_tabWidget->addTab(searchWidget,
                                       QIcon(":/images/find.png"),
                                       tabLabel);

    m_tabWidget->setTabToolTip(tabIndex, tabTooltip);
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
    SearchWidget *w = qobject_cast<SearchWidget*>(m_tabWidget->currentWidget());

    if(w) {
        w->toggleWidget();
    }
}

void SearchView::openNewTab()
{
    newTab(SearchWidget::LibrarySearch);
}
