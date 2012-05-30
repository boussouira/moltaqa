#include "viewmanager.h"
#include "mainwindow.h"
#include "abstarctview.h"
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

void ViewManager::addView(AbstarctView *view, bool selectable)
{
    view->setSelectable(selectable);

    addWidget(view);
    setupWindowsActions();

    m_viewDisplay.insert(view);

    connect(view, SIGNAL(hideMe()), SLOT(hideView()));
    connect(view, SIGNAL(showMe()), SLOT(showView()));
}

void ViewManager::removeView(AbstarctView *view)
{
    m_viewDisplay.remove(view);

    removeWidget(view);
    setupWindowsActions();
}

void ViewManager::setCurrentView(AbstarctView *view)
{
    AbstarctView *currentView = qobject_cast<AbstarctView*>(currentWidget());

    currentView->aboutToHide();

    foreach (QToolBar *bar, currentView->toolBars()) {
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
        bar->show();
    }

    if(m_navigationsMenu) {
        foreach (QAction*act, view->navigationActions())
            m_navigationsMenu->addAction(act);
    }

    view->updateToolBars();
    m_currentView = view;

    setupWindowsActions();
    m_mainWindow->activateWindow();
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

void ViewManager::hideView()
{
    AbstarctView *w = qobject_cast<AbstarctView*>(sender());
    if(w) {
        w->setSelectable(false);
        bool useDefautView = true;

        foreach(AbstarctView* prevView, m_viewDisplay) {
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
