#include "searchview.h"
#include "tabwidget.h"
#include "librarysearchwidget.h"
#include "booksearchwidget.h"
#include "mainwindow.h"
#include "indexmanager.h"
#include "librarybookmanager.h"
#include "stringutils.h"

#include <qboxlayout.h>
#include <qmessagebox.h>
#include <qaction.h>

SearchView::SearchView(QWidget *parent) : AbstarctView(parent)
{
    m_layout = new QVBoxLayout(this);
    m_tabWidget = new TabWidget(this);
    m_tabWidget->setAutoTabClose(true);

    m_layout->addWidget(m_tabWidget);
    m_layout->setContentsMargins(0, 6, 0, 0);

    QToolBar *bar = new QToolBar(tr("البحث"), this);

    QAction *actNewTab = new QAction(QIcon(":/images/tab-new.png"),
                               tr("تبويب بحث جديد"), this);
    QAction *actSwitchTab = new QAction(QIcon(":/images/switch.png"),
                                     tr("تنقل بين نافذة البحث والنتائج"), this);
    QAction *actSearchAgain = new QAction(QIcon(":/images/refresh.png"),
                                         tr("اعادة البحث"), this);
    QAction *actSearchInfo = new QAction(QIcon(":/images/about.png"),
                                          tr("نتائج البحث"), this);

    bar->addAction(actNewTab);
    bar->addAction(actSwitchTab);
    bar->addAction(actSearchAgain);
    bar->addAction(actSearchInfo);

    m_toolBars << bar;
    setLayout(m_layout);

    connect(actNewTab, SIGNAL(triggered()), SLOT(openNewTab()));
    connect(actSwitchTab, SIGNAL(triggered()), SLOT(switchSearchWidget()));
    connect(actSearchAgain, SIGNAL(triggered()), SLOT(searchAgain()));
    connect(actSearchInfo, SIGNAL(triggered()), SLOT(searchInfo()));
    connect(m_tabWidget, SIGNAL(lastTabClosed()), SIGNAL(hideMe()));
}

QString SearchView::title()
{
    return tr("نافذة البحث");
}

void SearchView::aboutToShow()
{
    if(!m_tabWidget->count())
        newTab(SearchWidget::LibrarySearch);
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

SearchWidget *SearchView::currentSearchWidget()
{
    return qobject_cast<SearchWidget*>(m_tabWidget->currentWidget());
}

SearchWidget *SearchView::newTab(SearchWidget::SearchType searchType, int bookID)
{
    ml_return_val_on_fail(canSearch(), 0);

    SearchWidget *searchWidget = 0;
    if(searchType == SearchWidget::LibrarySearch) {
        searchWidget = new LibrarySearchWidget(this);
    } else if(searchType == SearchWidget::BookSearch) {
        searchWidget = new BookSearchWidget(this);
    } else {
        qCritical("SearchView: Unknow search type: %d", searchType);
        return 0;
    }

    searchWidget->init(bookID);
    searchWidget->setAutoFillBackground(true);

    QString tabLabel = tr("بحث في المكتبة");
    QString tabTooltip = tabLabel;

    if(searchType == SearchWidget::BookSearch) {
        LibraryBookPtr book = LibraryManager::instance()->bookManager()->getLibraryBook(bookID);
        if(book) {
            QString bookName = book->title;
            QString shortBookName = Utils::String::abbreviate(bookName, 20);

            tabLabel = tr("بحث في %1").arg(shortBookName);
            tabTooltip = tr("بحث في %1").arg(bookName);
        }
    }

    int tabIndex = m_tabWidget->addTab(searchWidget,
                                       QIcon(":/images/find.png"),
                                       tabLabel);

    m_tabWidget->setTabToolTip(tabIndex, tabTooltip);
    m_tabWidget->setCurrentIndex(tabIndex);

    return searchWidget;
}

void SearchView::switchSearchWidget()
{
    SearchWidget *w = currentSearchWidget();
    ml_return_on_fail(w);

    w->toggleWidget();
}

void SearchView::openNewTab()
{
    newTab(SearchWidget::LibrarySearch);
}

void SearchView::searchAgain()
{
    SearchWidget *w = currentSearchWidget();
    ml_return_on_fail(w);
    ml_return_on_fail(w->currentWidget() == SearchWidget::Result);

    w->search();
}

void SearchView::searchInfo()
{
    SearchWidget *w = currentSearchWidget();
    ml_return_on_fail(w);
    ml_return_on_fail(w->currentWidget() == SearchWidget::Result);

    w->showSearchInfo();
}
