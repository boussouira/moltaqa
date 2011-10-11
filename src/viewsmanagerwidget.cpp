#include "viewsmanagerwidget.h"
#include "tabwidget.h"
#include "indexwidget.h"
#include "bookslistbrowser.h"
#include "simpledbhandler.h"
#include "qurandbhandler.h"
#include "tafessirdbhandler.h"
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

ViewsManagerWidget::ViewsManagerWidget(QWidget *parent) :
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

ViewsManagerWidget::~ViewsManagerWidget()
{
}

void ViewsManagerWidget::connectTab(TabWidget *tab)
{
    connect(tab, SIGNAL(currentChanged(int)), SLOT(tabChanged(int)));
    connect(tab, SIGNAL(currentChanged(int)), SIGNAL(currentTabChanged(int)));
    connect(tab, SIGNAL(tabCloseRequested(int)), SLOT(tabCloseRequest(int)));
    connect(tab, SIGNAL(gotFocus()), SLOT(changeActiveTab()));
    connect(tab, SIGNAL(moveToOtherTab(int)), SLOT(moveToOtherTab(int)));
    connect(tab, SIGNAL(reverseSplitter()), SLOT(reverseSplitter()));
}

void ViewsManagerWidget::tabCloseRequest(int tabIndex)
{
    setActiveTab(sender());
    bookWidget(tabIndex)->saveSettings();
//    QWidget *w = m_activeTab->widget(tabIndex);
    m_activeTab->removeTab(tabIndex);

//    delete w;
}

void ViewsManagerWidget::tabChanged(int newIndex)
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

void ViewsManagerWidget::changeActiveTab()
{
    setActiveTab(sender());
}

void ViewsManagerWidget::addBook(BookWidget *book)
{
    int tabIndex = m_activeTab->addBookWidget(book);

    m_activeTab->setCurrentIndex(tabIndex);
}

BookWidget *ViewsManagerWidget::bookWidget(int index)
{
    return qobject_cast<BookWidget*>(m_activeTab->widget(index));
}

BookWidget *ViewsManagerWidget::activeBook()
{
    return qobject_cast<BookWidget*>(m_activeTab->widget(m_activeTab->currentIndex()));
}

BookInfo *ViewsManagerWidget::activeBookInfo()
{
    return activeBook()->dbHandler()->bookInfo();
}

RichBookReader *ViewsManagerWidget::activeDBHandler()
{
    return activeBook()->dbHandler();
}

TabWidget *ViewsManagerWidget::activeTab()
{
    return m_activeTab;
}

void ViewsManagerWidget::moveToOtherTab(int index)
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
        m_showOtherTab = true;
        tabChanged(0);

        active->blockSignals(false);
        otherTab->blockSignals(false);
    }
}

void ViewsManagerWidget::setActiveTab(QObject *obj)
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

void ViewsManagerWidget::reverseSplitter()
{
    m_splitter->setOrientation((m_splitter->orientation()==Qt::Horizontal) ?
                                   Qt::Vertical : Qt::Horizontal);
}

