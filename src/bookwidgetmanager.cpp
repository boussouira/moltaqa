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
#include "utils.h"
#include "favouritesmanager.h"

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
#include <qapplication.h>
#include <qclipboard.h>
#include <qstatusbar.h>

BookWidgetManager::BookWidgetManager(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_splitter = new QSplitter(this);
    m_topTab = new TabWidget(this);
    m_bottomTab = new TabWidget(this);

    m_libraryManager = LibraryManager::instance();

    m_activeTab = m_topTab;

    m_splitter->addWidget(m_topTab);
    m_splitter->addWidget(m_bottomTab);

    m_splitter->setOrientation(Qt::Vertical);

    m_moveAct = new QAction(tr("نقل الى نافذة اخرى"), this);
    m_revAct = new QAction(tr("عكس تجاور النوافذ"), this);
    m_favouriteAct = new QAction(QIcon::fromTheme("bookmark-new", QIcon(":/images/bookmark-new.png")),
                                                  tr("اضافة الى المفضلة"), this);

    m_showOtherTab = true;

    layout->addWidget(m_splitter);
    layout->setContentsMargins(0,0,0,0);

    connectTab(m_topTab);
    connectTab(m_bottomTab);

    connect(m_moveAct, SIGNAL(triggered()), SLOT(moveToOtherTab()));
    connect(m_revAct, SIGNAL(triggered()), SLOT(reverseSplitter()));
    connect(m_favouriteAct, SIGNAL(triggered()), SLOT(addToFavouite()));
}

BookWidgetManager::~BookWidgetManager()
{
}

void BookWidgetManager::connectTab(TabWidget *tab)
{
    tab->setTabBarActions(QList<QAction*>()
                          << m_moveAct << m_revAct << 0 << m_favouriteAct);

    connect(tab, SIGNAL(currentChanged(int)), SLOT(tabChanged(int)));
    connect(tab, SIGNAL(currentChanged(int)), SIGNAL(currentTabChanged(int)));
    connect(tab, SIGNAL(tabCloseRequested(int)), SLOT(tabCloseRequest(int)));
    connect(tab, SIGNAL(gotFocus()), SLOT(changeActiveTab()));
}

void BookWidgetManager::reverseActiveTab()
{
    setActiveTab(unActiveTab());
}

void BookWidgetManager::tabCloseRequest(int tabIndex)
{
    setActiveTab(sender());
    bookWidget(tabIndex)->saveSettings();
    QWidget *w = m_activeTab->widget(tabIndex);
    m_activeTab->removeTab(tabIndex);

    if(!m_activeTab->count() && unActiveTab()->count())
        reverseActiveTab();

    ML_DELETE_CHECK(w);
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
    m_activeTab->setTabIcon(tabIndex, QIcon(":/images/book.png"));

    connect(book, SIGNAL(textChanged()), SIGNAL(pageChanged()));

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

LibraryBookPtr BookWidgetManager::activeBook()
{
    BookWidget *bookWidget = activeBookWidget();
    return bookWidget ? bookWidget->bookReader()->bookInfo() : LibraryBookPtr();
}

RichBookReader *BookWidgetManager::activeBookReader()
{
    BookWidget *bookWidget = activeBookWidget();
    return bookWidget ? bookWidget->bookReader() : 0;
}

TabWidget *BookWidgetManager::activeTab()
{
    return m_activeTab;
}

TabWidget *BookWidgetManager::unActiveTab()
{
    return (m_activeTab == m_topTab ? m_bottomTab : m_topTab);
}

void BookWidgetManager::moveToOtherTab()
{
    TabWidget *active = activeTab();
    ML_ASSERT2(active, "BookWidgetManager::moveToOtherTab active tab is null");

    int index = active->currentIndex();
    ML_ASSERT2(index != -1, "BookWidgetManager::moveToOtherTab wrong tab index");

    TabWidget *otherTab = (active == m_topTab) ? m_bottomTab : m_topTab;
    BookWidget *book = bookWidget(index);
    ML_ASSERT2(book, "BookWidgetManager::moveToOtherTab book widget is null");

    active->blockSignals(true);
    otherTab->blockSignals(true);

    active->removeTab(index);
    int i = otherTab->addBookWidget(book);
    otherTab->setCurrentIndex(i);
    m_showOtherTab = true;
    tabChanged(0);

    active->blockSignals(false);
    otherTab->blockSignals(false);

    reverseActiveTab();
}

void BookWidgetManager::setActiveTab(QObject *obj)
{
    if(obj != m_activeTab) {
        TabWidget *tab = qobject_cast<TabWidget*>(obj);
        if(tab) {
            //static int count = 0;
            //qDebug("%d - changeActiveTab: %s", count++, (tab==m_topTab)?"TOP":"BOTTOM");
            m_activeTab = tab;
            emit currentTabChanged(m_activeTab->currentIndex());

            m_activeTab->setEnableTabBar(true);
            unActiveTab()->setEnableTabBar(false);
        }
    }
}

void BookWidgetManager::reverseSplitter()
{
    m_splitter->setOrientation((m_splitter->orientation()==Qt::Horizontal) ?
                                   Qt::Vertical : Qt::Horizontal);
}

void BookWidgetManager::addToFavouite()
{
    LibraryBookPtr book = activeBook();
    ML_ASSERT(book);

    if(m_libraryManager->favouritesManager()->containsBook(book->bookID)) {
        QMessageBox::information(this,
                                 tr("المفضلة"),
                                 tr("الكتاب <strong>%1</strong> موجودة في قائمة المفضلة")
                                 .arg(book->bookDisplayName));
    } else {
        m_libraryManager->favouritesManager()->addBook(book->bookID, 0);
        m_libraryManager->favouritesManager()->reloadModels();

        QMessageBox::information(this,
                                 tr("المفضلة"),
                                 tr("تم اضافة <strong>%1</strong> الى المفضلة")
                                 .arg(book->bookDisplayName));
    }
}

void BookWidgetManager::closeBook(int bookID)
{
    for(int i=m_topTab->count()-1; i>=0; i--) {
        BookWidget *bookWidget = qobject_cast<BookWidget*>(m_topTab->widget(i));
        if(bookWidget) {
            if(bookWidget->bookReader()->bookInfo()->bookID == bookID) {
                QObject::metaObject()->invokeMethod(m_topTab, "tabCloseRequested",
                                                    Q_ARG(int, i));
            }
        }
    }

    for(int i=m_bottomTab->count()-1; i>=0; i--) {
        BookWidget *bookWidget = qobject_cast<BookWidget*>(m_bottomTab->widget(i));
        if(bookWidget) {
            if(bookWidget->bookReader()->bookInfo()->bookID == bookID) {
                QObject::metaObject()->invokeMethod(m_bottomTab, "tabCloseRequested",
                                                    Q_ARG(int, i));
            }
        }
    }
}

BookWidget *BookWidgetManager::getBookWidget(int bookID)
{
    for(int i=0; i<m_topTab->count(); i++) {
        BookWidget *bookWidget = qobject_cast<BookWidget*>(m_topTab->widget(i));
        if(bookWidget) {
            if(bookWidget->bookReader()->bookInfo()->bookID == bookID) {
                return bookWidget;
            }
        }
    }

    for(int i=0; i<m_bottomTab->count(); i++) {
        BookWidget *bookWidget = qobject_cast<BookWidget*>(m_bottomTab->widget(i));
        if(bookWidget) {
            if(bookWidget->bookReader()->bookInfo()->bookID == bookID) {
                return bookWidget;
            }
        }
    }

    return 0;
}

void BookWidgetManager::nextAya()
{
    activeBookWidget()->scrollDown();
}

void BookWidgetManager::previousAya()
{
    activeBookWidget()->scrollUp();
}

void BookWidgetManager::nextPage()
{
    activeBookWidget()->nextPage();
}

void BookWidgetManager::previousPage()
{
    activeBookWidget()->prevPage();
}

void BookWidgetManager::firstPage()
{
    activeBookWidget()->firstPage();
}

void BookWidgetManager::lastPage()
{
    activeBookWidget()->lastPage();
}

void BookWidgetManager::goToPage()
{
    OpenPageDialog dialog(this);
    dialog.setInfo(activeBook(), activeBookReader()->page());

    if(dialog.exec() == QDialog::Accepted) {
        if(dialog.currentPage() == 0) // Open selected page/part
            activeBookWidget()->openPage(dialog.selectedPage(), dialog.selectedPart());
        else if(dialog.currentPage() == 1) // Open selected sora/page
            activeBookWidget()->openSora(dialog.selectedSora(), dialog.selectedAya());
        else if(dialog.currentPage() == 2) // Open selected haddit
            activeBookWidget()->openHaddit(dialog.selectedHaddit());
        else
            qDebug("What to do?");
    }
}

void BookWidgetManager::copyPageLink()
{
    AbstractBookReader *reader = activeBookReader();
    if(reader) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(QString("%1:%2")
                           .arg(reader->bookInfo()->bookID)
                           .arg(reader->page()->pageID));

        MW->statusBar()->showMessage(tr("تم نسخ معرف الصفحة"),
                                     1000);
    }
}
