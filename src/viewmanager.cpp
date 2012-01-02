#include "viewmanager.h"
#include "mainwindow.h"
#include "abstarctview.h"
#include <qmenu.h>

ViewManager::ViewManager(QWidget *parent) :
    QStackedWidget(parent)
{
    m_mainWindow = MW;
    m_defautView = 0;
    m_currentView = 0;
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

    foreach (QToolBar *bar, currentView->toolBars()) {
        m_mainWindow->removeToolBar(bar);
    }

    foreach (QAction*act, currentView->navigationActions())
        m_navigationsMenu->removeAction(act);

    setCurrentWidget(view);

    foreach (QToolBar *bar, view->toolBars()) {
        m_mainWindow->addToolBar(bar);
        bar->show();
    }

    foreach (QAction*act, view->navigationActions())
        m_navigationsMenu->addAction(act);

    m_navigationsMenu->setEnabled(!view->navigationActions().isEmpty());

    view->updateToolBars();
    m_currentView = view;

    setupWindowsActions();
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
