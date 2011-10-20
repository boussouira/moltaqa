#include "abstarctview.h"

AbstarctView::AbstarctView(QWidget *parent) :
    QWidget(parent),
    m_selectable(true)
{
}

QList<QToolBar*> AbstarctView::toolBars()
{
    return m_toolBars;
}

void AbstarctView::showToolBars()
{
    foreach(QToolBar *bar, m_toolBars) {
        bar->show();
    }
}

void AbstarctView::hideMenu()
{
}

void AbstarctView::showMenu()
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
