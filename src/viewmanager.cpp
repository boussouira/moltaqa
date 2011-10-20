#include "viewmanager.h"
#include "mainwindow.h"
#include "abstarctview.h"
#include <qmenu.h>

ViewManager::ViewManager(QWidget *parent) :
    QStackedWidget(parent)
{
    m_mainWindow = MW;
}

void ViewManager::addView(AbstarctView *view)
{
    addWidget(view);
    setupActions();
}

void ViewManager::removeView(AbstarctView *view)
{
    removeWidget(view);
    setupActions();
}

void ViewManager::setCurrentView(int index)
{
    AbstarctView *currentView;

    currentView = qobject_cast<AbstarctView*>(currentWidget());
    foreach (QToolBar *bar, currentView->toolBars()) {
        m_mainWindow->removeToolBar(bar);
    }

    currentView->hideMenu();

    setCurrentIndex(index);

    currentView = qobject_cast<AbstarctView*>(currentWidget());
    foreach (QToolBar *bar, currentView->toolBars()) {
        m_mainWindow->addToolBar(bar);
    }

    currentView->showToolBars();
    currentView->showMenu();

    setupActions();
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
