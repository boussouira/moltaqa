#include "bookwidgetmanager.h"
#include "bookhistorydialog.h"
#include "bookinfodialog.h"
#include "bookslistbrowser.h"
#include "bookviewbase.h"
#include "favouritesmanager.h"
#include "indexwidget.h"
#include "librarymanager.h"
#include "mainwindow.h"
#include "openpagedialog.h"
#include "richquranreader.h"
#include "richsimplebookreader.h"
#include "richtafessirreader.h"
#include "tabwidget.h"
#include "utils.h"

#include <qaction.h>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qclipboard.h>
#include <qcombobox.h>
#include <qmainwindow.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qsplitter.h>
#include <qstatusbar.h>
#include <qtoolbar.h>

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

    Qt::Orientation orientation = static_cast<Qt::Orientation>(Utils::Settings::get("BookWidgetManager/splitter",
                                                                                    Qt::Vertical).toInt());
    m_splitter->setOrientation(orientation);

    m_moveAct = new QAction(tr("نقل الى نافذة اخرى"), this);
    m_revAct = new QAction(tr("عكس تجاور النوافذ"), this);
    m_favouriteAct = new QAction(ml_theme_icon("bookmark-new", ":/images/bookmark-new.png"),
                                 tr("اضافة الى المفضلة"), this);

    m_bookInfoAct = new QAction(tr("بطاقة الكتاب"), this);

    m_showOtherTab = true;

    layout->addWidget(m_splitter);
    layout->setContentsMargins(0,0,0,0);

    connectTab(m_topTab);
    connectTab(m_bottomTab);

    connect(m_moveAct, SIGNAL(triggered()), SLOT(moveToOtherTab()));
    connect(m_revAct, SIGNAL(triggered()), SLOT(reverseSplitter()));
    connect(m_favouriteAct, SIGNAL(triggered()), SLOT(addToFavouite()));
    connect(m_bookInfoAct, SIGNAL(triggered()), SLOT(showBookInfo()));
}

BookWidgetManager::~BookWidgetManager()
{
}

void BookWidgetManager::connectTab(TabWidget *tab)
{
    QList<QAction*> actionsList;
    actionsList << m_moveAct << m_revAct << 0
                << m_bookInfoAct << m_favouriteAct;

    tab->setTabBarActions(actionsList);

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

    ml_delete_check(w);
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

int BookWidgetManager::addBook(BookViewBase *book)
{
    int tabIndex = m_activeTab->addBookWidget(book);
    m_activeTab->setCurrentIndex(tabIndex);
    m_activeTab->setTabIcon(tabIndex, QIcon(":/images/book.png"));

    connect(book, SIGNAL(textChanged()), SIGNAL(pageChanged()));

    return tabIndex;
}

BookViewBase *BookWidgetManager::bookWidget(int index)
{
    return qobject_cast<BookViewBase*>(m_activeTab->widget(index));
}

BookViewBase *BookWidgetManager::activeBookWidget()
{
    return qobject_cast<BookViewBase*>(m_activeTab->widget(m_activeTab->currentIndex()));
}

LibraryBook::Ptr BookWidgetManager::activeBook()
{
    BookViewBase *bookWidget = activeBookWidget();
    return bookWidget ? bookWidget->book() : LibraryBook::Ptr();
}

AbstractBookReader *BookWidgetManager::activeBookReader()
{
    BookViewBase *bookWidget = activeBookWidget();
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
    ml_return_on_fail2(active, "BookWidgetManager::moveToOtherTab active tab is null");

    int index = active->currentIndex();
    ml_return_on_fail2(index != -1, "BookWidgetManager::moveToOtherTab wrong tab index");

    TabWidget *otherTab = (active == m_topTab) ? m_bottomTab : m_topTab;
    BookViewBase *book = bookWidget(index);
    ml_return_on_fail2(book, "BookWidgetManager::moveToOtherTab book widget is null");

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

    Utils::Settings::set("BookWidgetManager/splitter", m_splitter->orientation());
}

void BookWidgetManager::addToFavouite()
{
    LibraryBook::Ptr book = activeBook();
    ml_return_on_fail(book);

    if(m_libraryManager->favouritesManager()->containsBook(book->id)) {
        QMessageBox::information(this,
                                 tr("المفضلة"),
                                 tr("الكتاب <strong>%1</strong> موجودة في قائمة المفضلة")
                                 .arg(book->title));
    } else {
        m_libraryManager->favouritesManager()->addBook(book->id, 0);
        m_libraryManager->favouritesManager()->reloadModels();

        QMessageBox::information(this,
                                 tr("المفضلة"),
                                 tr("تم اضافة <strong>%1</strong> الى المفضلة")
                                 .arg(book->title));
    }
}

void BookWidgetManager::showBookInfo()
{
    LibraryBook::Ptr book = activeBook();
    ml_return_on_fail(book);

    BookInfoDialog *dialog = new BookInfoDialog(0);
    dialog->setLibraryBook(book);
    dialog->setup();
    dialog->show();
}

void BookWidgetManager::showBookHistory()
{
    LibraryBook::Ptr book = activeBook();
    BookViewBase *widget = activeBookWidget();
    ml_return_on_fail(book && widget);

    BookHistoryDialog *dialog = new BookHistoryDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setLibraryBook(book);
    dialog->setup();
    dialog->show();

    connect(dialog, SIGNAL(openPage(int)), widget, SLOT(openPage(int)));
}

void BookWidgetManager::closeBook(int bookID)
{
    for(int i=m_topTab->count()-1; i>=0; i--) {
        BookViewBase *bookWidget = qobject_cast<BookViewBase*>(m_topTab->widget(i));
        if(bookWidget) {
            if(bookWidget->book()->id == bookID) {
                QObject::metaObject()->invokeMethod(m_topTab, "tabCloseRequested",
                                                    Q_ARG(int, i));
            }
        }
    }

    for(int i=m_bottomTab->count()-1; i>=0; i--) {
        BookViewBase *bookWidget = qobject_cast<BookViewBase*>(m_bottomTab->widget(i));
        if(bookWidget) {
            if(bookWidget->book()->id == bookID) {
                QObject::metaObject()->invokeMethod(m_bottomTab, "tabCloseRequested",
                                                    Q_ARG(int, i));
            }
        }
    }
}

BookViewBase *BookWidgetManager::getBookWidget(int bookID)
{
    for(int i=0; i<m_topTab->count(); i++) {
        BookViewBase *bookWidget = qobject_cast<BookViewBase*>(m_topTab->widget(i));
        if(bookWidget) {
            if(bookWidget->book()->id == bookID) {
                return bookWidget;
            }
        }
    }

    for(int i=0; i<m_bottomTab->count(); i++) {
        BookViewBase *bookWidget = qobject_cast<BookViewBase*>(m_bottomTab->widget(i));
        if(bookWidget) {
            if(bookWidget->book()->id == bookID) {
                return bookWidget;
            }
        }
    }

    return 0;
}

QList<BookViewBase *> BookWidgetManager::getBookWidgets()
{
    QList<BookViewBase *> list;

    for(int i=0; i<m_topTab->count(); i++) {
        BookViewBase *bookWidget = qobject_cast<BookViewBase*>(m_topTab->widget(i));
        if(bookWidget) {
            list.append(bookWidget);
        }
    }

    for(int i=0; i<m_bottomTab->count(); i++) {
        BookViewBase *bookWidget = qobject_cast<BookViewBase*>(m_bottomTab->widget(i));
        if(bookWidget) {
            list.append(bookWidget);
        }
    }

    return list;
}

bool BookWidgetManager::showBook(int bookID)
{
    for(int i=0; i<m_topTab->count(); i++) {
        BookViewBase *bookWidget = qobject_cast<BookViewBase*>(m_topTab->widget(i));
        if(bookWidget && bookWidget->book()->id == bookID) {
            setActiveTab(m_topTab);
            m_topTab->setCurrentIndex(i);

            return true;
        }
    }

    for(int i=0; i<m_bottomTab->count(); i++) {
        BookViewBase *bookWidget = qobject_cast<BookViewBase*>(m_bottomTab->widget(i));
        if(bookWidget && bookWidget->book()->id == bookID) {
            setActiveTab(m_bottomTab);
            m_bottomTab->setCurrentIndex(i);

            return true;
        }
    }

    return false;
}

void BookWidgetManager::addTabActions(QList<QAction*> tabActions)
{
    QList<QAction*> topActions = m_topTab->tabActions();
    topActions.append(tabActions);
    m_topTab->setTabBarActions(topActions);

    QList<QAction*> bottomActions = m_bottomTab->tabActions();
    bottomActions.append(tabActions);
    m_bottomTab->setTabBarActions(bottomActions);

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
            qDebug("BookWidgetManager::goToPage unknow page index %d", dialog.currentPage());
    }
}
