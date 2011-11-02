#include "viewmanager.h"
#include "mainwindow.h"
#include "abstarctview.h"
#include <qmenu.h>

ViewManager::ViewManager(QWidget *parent) :
    QStackedWidget(parent)
{
    m_mainWindow = MW;
    m_defautView = 0;
}

void ViewManager::addView(AbstarctView *view, bool selectable)
{
    view->setSelectable(selectable);

    addWidget(view);
    setupActions();

    m_viewDisplay.push(view);

    connect(view, SIGNAL(hideMe()), SLOT(hideView()));
}

void ViewManager::removeView(AbstarctView *view)
{
    removeViewFromStack(view);

    removeWidget(view);
    setupActions();
}

void ViewManager::setCurrentView(AbstarctView *view)
{
    AbstarctView *currentView = qobject_cast<AbstarctView*>(currentWidget());
    foreach (QToolBar *bar, currentView->toolBars()) {
        m_mainWindow->removeToolBar(bar);
    }

    currentView->hideMenu();

    setCurrentWidget(view);

    foreach (QToolBar *bar, view->toolBars()) {
        m_mainWindow->addToolBar(bar);
    }

    view->showToolBars();
    view->showMenu();

    setupActions();
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

void ViewManager::setMenu(QMenu *menu)
{
    m_menu = menu;
}

void ViewManager::setupActions()
{
    m_menu->clear();

    for(int i=0; i < count(); i++) {
        QAction *act = new QAction(m_menu);
        AbstarctView *view = qobject_cast<AbstarctView*>(widget(i));
        if(view && view->isSelectable()) {
            act->setText(view->title());
            act->setData(i);
            act->setCheckable(true);
            act->setChecked(i == currentIndex());

            connect(act, SIGNAL(triggered(bool)), SLOT(changeWindow()));
            m_menu->addAction(act);
        }
    }
}

void ViewManager::removeViewFromStack(AbstarctView *view)
{
    for(int i=0; i<m_viewDisplay.size(); i++) {
        if(m_viewDisplay[i] == view) {
            m_viewDisplay.remove(i);
        }
    }
}

void ViewManager::addViewToStack(AbstarctView *view)
{
    removeViewFromStack(view);
    m_viewDisplay.push(view);
}

void ViewManager::changeWindow()
{
    QAction *act = qobject_cast<QAction*>(sender());
    if(act) {
        if(!act->isChecked()) {
            act->setChecked(true);
            return;
        }

        foreach(QAction *a, m_menu->actions()) {
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
        while(!m_viewDisplay.isEmpty()) {
            AbstarctView *prevView = m_viewDisplay.pop();
            if(prevView && prevView != w && prevView->isSelectable()) {
                setCurrentView(prevView);
                useDefautView = false;
            }
        }

        if(useDefautView)
            setCurrentView(m_defautView);
    }
}