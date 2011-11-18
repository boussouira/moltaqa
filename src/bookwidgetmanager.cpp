#include "bookwidgetmanager.h"
#include "tabwidget.h"
#include "indexwidget.h"
#include "bookslistbrowser.h"
#include "richsimplebookreader.h"
#include "richquranreader.h"
#include "richtafessirreader.h"
#include "librarymanager.h"
#include "bookwidget.h"
#include "bookexception.h"
#include "openpagedialog.h"
#include "mainwindow.h"

#include <qmainwindow.h>
#include <qmenubar.h>
#include <qmenu.h>
#include <qaction.h>
#include <qtoolbar.h>
#include <qcombobox.h>
#include <qsplitter.h>
#include <qboxlayout.h>
#include <qdebug.h>
#include <qmessagebox.h>

BookWidgetManager::BookWidgetManager(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_splitter = new QSplitter(this);
    m_topTab = new TabWidget(this);
    m_bottomTab = new TabWidget(this);

    m_activeTab = m_topTab;

    m_splitter->addWidget(m_topTab);
    m_splitter->addWidget(m_bottomTab);

    m_splitter->setOrientation(Qt::Vertical);

    m_showOtherTab = true;

    layout->addWidget(m_splitter);
    layout->setContentsMargins(0,0,0,0);

    connectTab(m_topTab);
    connectTab(m_bottomTab);
}

BookWidgetManager::~BookWidgetManager()
{
}

void BookWidgetManager::connectTab(TabWidget *tab)
{
    connect(tab, SIGNAL(currentChanged(int)), SLOT(tabChanged(int)));
    connect(tab, SIGNAL(currentChanged(int)), SIGNAL(currentTabChanged(int)));
    connect(tab, SIGNAL(tabCloseRequested(int)), SLOT(tabCloseRequest(int)));
    connect(tab, SIGNAL(gotFocus()), SLOT(changeActiveTab()));
    connect(tab, SIGNAL(moveToOtherTab(int)), SLOT(moveToOtherTab(int)));
    connect(tab, SIGNAL(reverseSplitter()), SLOT(reverseSplitter()));
}

void BookWidgetManager::reverseActiveTab()
{
    setActiveTab(m_activeTab == m_topTab ? m_bottomTab : m_topTab);
}

void BookWidgetManager::tabCloseRequest(int tabIndex)
{
    setActiveTab(sender());
    bookWidget(tabIndex)->saveSettings();
    QWidget *w = m_activeTab->widget(tabIndex);
    m_activeTab->removeTab(tabIndex);

    if(m_activeTab->count() <= 0)
        reverseActiveTab();

    delete w;
}

void BookWidgetManager::tabChanged(int newIndex)
{
    Q_UNUSED(newIndex);

    if(!m_topTab->count() && !m_bottomTab->count()) {
        emit lastTabClosed();
    } else if(!m_topTab->count()) {
        QList<int> size;
        size << 0 << 1;
        m_splitter->setSizes(size);
    } else if(!m_bottomTab->count()) {
        QList<int> size;
        size << 1 << 0;
        m_splitter->setSizes(size);
    } else {
        if(m_splitter->sizes().contains(0) && m_showOtherTab) {
            QList<int> size;
            size << 1 << 1;
            m_splitter->setSizes(size);
            m_showOtherTab = false;
        }
    }
}

void BookWidgetManager::changeActiveTab()
{
    setActiveTab(sender());
}

int BookWidgetManager::addBook(BookWidget *book)
{
    int tabIndex = m_activeTab->addBookWidget(book);
    m_activeTab->setCurrentIndex(tabIndex);
    m_activeTab->setTabIcon(tabIndex, book->icon());

    connect(book, SIGNAL(setIcon(QIcon)), SLOT(changeTabIcon(QIcon)));

    return tabIndex;
}

BookWidget *BookWidgetManager::bookWidget(int index)
{
    return qobject_cast<BookWidget*>(m_activeTab->widget(index));
}

BookWidget *BookWidgetManager::activeBookWidget()
{
    return qobject_cast<BookWidget*>(m_activeTab->widget(m_activeTab->currentIndex()));
}

LibraryBook *BookWidgetManager::activeBook()
{
    BookWidget *bookWidget = activeBookWidget();
    return bookWidget ? bookWidget->bookReader()->bookInfo() : 0;
}

RichBookReader *BookWidgetManager::activeBookReader()
{
    return activeBookWidget()->bookReader();
}

TabWidget *BookWidgetManager::activeTab()
{
    return m_activeTab;
}

void BookWidgetManager::moveToOtherTab(int index)
{
    setActiveTab(sender());

    TabWidget *active = qobject_cast<TabWidget*>(sender());
    if(active) {
        TabWidget *otherTab = (active == m_topTab) ? m_bottomTab : m_topTab;
        BookWidget *book = bookWidget(index);

        active->blockSignals(true);
        otherTab->blockSignals(true);

        active->removeTab(index);
        int i = otherTab->addBookWidget(book);
        otherTab->setCurrentIndex(i);
        otherTab->setTabIcon(i, book->icon());
        m_showOtherTab = true;
        tabChanged(0);

        active->blockSignals(false);
        otherTab->blockSignals(false);

        reverseActiveTab();
    }
}

void BookWidgetManager::setActiveTab(QObject *obj)
{
    if(obj != m_activeTab) {
        TabWidget *tab = qobject_cast<TabWidget*>(obj);
        if(tab) {
            //qDebug("changeActiveTab: %s", (tab==m_topTab)?"TOP":"BOTTOM");
            m_activeTab = tab;
            emit currentTabChanged(m_activeTab->currentIndex());
        }
    }
}

void BookWidgetManager::reverseSplitter()
{
    m_splitter->setOrientation((m_splitter->orientation()==Qt::Horizontal) ?
                                   Qt::Vertical : Qt::Horizontal);
}

void BookWidgetManager::changeTabIcon(QIcon tabIcon)
{
    QWidget *tab = qobject_cast<QWidget*>(sender());
    if(tab) {
        for(int i=0; i<m_topTab->count(); i++) {
            if(m_topTab->widget(i) == tab) {
                m_topTab->setTabIcon(i, tabIcon);
                return;
            }
        }

        for(int i=0; i<m_bottomTab->count(); i++) {
            if(m_bottomTab->widget(i) == tab) {
                m_bottomTab->setTabIcon(i, tabIcon);
                return;
            }
        }
    }
}

