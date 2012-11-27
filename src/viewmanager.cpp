#include "viewmanager.h"
#include "mainwindow.h"
#include "abstarctview.h"
#include "windowsview.h"
#include "utils.h"

#include <qmenu.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qstatusbar.h>

ViewManager::ViewManager(QWidget *parent) :
    QStackedWidget(parent),
     m_mainWindow(MW),
     m_windowsMenu(0),
     m_navigationsMenu(0),
     m_copyLinkAction(0),
     m_defautView(0),
     m_currentView(0)
{
    m_copyLinkAction = new QAction(tr("نسخ رابط الشاشة"), this);

    connect(m_copyLinkAction, SIGNAL(triggered()), SLOT(copyViewLink()));
}

void ViewManager::aboutToClose()
{
    AbstarctView *currentView = qobject_cast<AbstarctView*>(currentWidget());
    ml_return_on_fail(currentView);

    foreach (QToolBar *bar, currentView->toolBars()) {
        Utils::Widget::save(bar);
    }
}

void ViewManager::addView(AbstarctView *view, bool selectable)
{
    view->setSelectable(selectable);

    addWidget(view);
    setupWindowsActions();

    m_viewDisplay.append(view);

    connect(view, SIGNAL(hideMe()), SLOT(hideView()));
    connect(view, SIGNAL(showMe()), SLOT(showView()));
}

void ViewManager::removeView(AbstarctView *view)
{
    m_viewDisplay.removeAll(view);

    removeWidget(view);
    setupWindowsActions();
}

void ViewManager::setCurrentView(AbstarctView *view)
{
    AbstarctView *currentView = qobject_cast<AbstarctView*>(currentWidget());

    currentView->aboutToHide();

    foreach (QToolBar *bar, currentView->toolBars()) {
        Utils::Widget::save(bar);
        m_mainWindow->removeToolBar(bar);
    }

    if(m_navigationsMenu) {
        foreach (QAction*act, currentView->navigationActions())
            m_navigationsMenu->removeAction(act);
    }

    view->setSelectable(true);
    view->aboutToShow();

    setCurrentWidget(view);

    foreach (QToolBar *bar, view->toolBars()) {
        m_mainWindow->addToolBar(bar);
        Utils::Widget::restore(bar);
    }

    if(m_navigationsMenu) {
        foreach (QAction*act, view->navigationActions())
            m_navigationsMenu->addAction(act);
    }

    view->updateToolBars();
    m_currentView = view;

    setupWindowsActions();
    m_mainWindow->activateWindow();

    m_viewDisplay.removeAll(view);
    m_viewDisplay.append(view);
}

void ViewManager::setDefautView(AbstarctView *view)
{
    m_defautView = view;
}

void ViewManager::setCurrentView(int index)
{
    AbstarctView *view = qobject_cast<AbstarctView*>(widget(index));
    if(view)
        setCurrentView(view);
}

void ViewManager::setWindowsMenu(QMenu *menu)
{
    m_windowsMenu = menu;
}

void ViewManager::setNavigationMenu(QMenu *menu)
{
    m_navigationsMenu = menu;
    menu->addAction(m_copyLinkAction);
}

void ViewManager::setupWindowsActions()
{
    m_windowsMenu->clear();

    int selectableCount = 0;
    for(int i=0; i < count(); i++) {
        QAction *act = new QAction(m_windowsMenu);
        AbstarctView *view = qobject_cast<AbstarctView*>(widget(i));
        if(view && view->isSelectable()) {
            act->setText(view->title());
            act->setData(i);
            act->setCheckable(true);
            act->setChecked(i == currentIndex());
            act->setShortcut(QKeySequence(QString("ALT+%1").arg(++selectableCount)));

            connect(act, SIGNAL(triggered(bool)), SLOT(changeWindow()));
            m_windowsMenu->addAction(act);
        }
    }

    m_windowsMenu->addSeparator();
    QAction *act = m_windowsMenu->addAction(tr("عرض كل النوافذ"),
                                            this, SLOT(showAllWindows()));
    act->setShortcut(QKeySequence(("CTRL+TAB")));
}

void ViewManager::changeWindow()
{
    QAction *act = qobject_cast<QAction*>(sender());
    if(act) {
        if(!act->isChecked()) {
            act->setChecked(true);
            return;
        }

        foreach(QAction *a, m_windowsMenu->actions()) {
            if(act != a) {
                a->setChecked(false);
            }
        }

        setCurrentView(act->data().toInt());
    }
}

void ViewManager::copyViewLink()
{
    QString link = m_currentView->viewLink();
    if(link.isEmpty()) {
        m_mainWindow->statusBar()->showMessage(tr("لا يمكن نسخ رابط هذه الشاشة"),
                                               2500);
    } else {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(link);

        m_mainWindow->statusBar()->showMessage(tr("تم نسخ الرابط: %1").arg(link), 2500);
    }
}

void ViewManager::showAllWindows()
{
    WindowsView view(this);

    for(int i=m_viewDisplay.size()-1; i>=0; i--) {
        AbstarctView* prevView = m_viewDisplay.at(i);
        if(prevView->isSelectable()) {
            view.addView(prevView, i);
        }
    }

    connect(&view, SIGNAL(selectView(QString)), SLOT(selectViewFromListIndex(QString)));

    view.selectCurrentView();
    view.exec();
}

void ViewManager::selectViewFromListIndex(QString title)
{
    for(int i=0; i<m_viewDisplay.size(); i++) {
    AbstarctView *view = qobject_cast<AbstarctView*>(m_viewDisplay.at(i));
    if(view && view->title() == title)
        setCurrentView(view);
    }
}

void ViewManager::hideView()
{
    AbstarctView *w = qobject_cast<AbstarctView*>(sender());
    if(w) {
        w->setSelectable(false);
        bool useDefautView = true;

        for(int i=m_viewDisplay.size()-1; i>=0; i--) {
            AbstarctView* prevView = m_viewDisplay.at(i);
//        foreach(AbstarctView* prevView, m_viewDisplay) {
            if(prevView && prevView != w && prevView->isSelectable() && prevView != m_defautView) {
                setCurrentView(prevView);
                useDefautView = false;
                break;
            }
        }

        if(useDefautView)
            setCurrentView(m_defautView);
    }
}

void ViewManager::showView()
{
    AbstarctView *w = qobject_cast<AbstarctView*>(sender());
    if(w) {
        if(m_currentView != w) {
            w->setSelectable(true);
            setCurrentView(w);
        }
    }
}
