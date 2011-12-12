#include "abstarctview.h"
#include <qaction.h>

AbstarctView::AbstarctView(QWidget *parent) :
    QWidget(parent),
    m_selectable(true)
{
}

QList<QToolBar*> AbstarctView::toolBars()
{
    return m_toolBars;
}

QList<QAction *> AbstarctView::navigationActions()
{
    return m_navActions;
}

void AbstarctView::updateToolBars()
{
}

void AbstarctView::updateActions()
{
}

bool AbstarctView::isSelectable()
{
    return m_selectable;
}

void AbstarctView::setSelectable(bool selectebale)
{
    m_selectable = selectebale;
}

QAction *AbstarctView::actionSeparator(QObject *parent)
{
    QAction *act = new QAction(parent);
    act->setSeparator(true);

    return act;
}
