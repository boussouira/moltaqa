#include "controlcenterwidget.h"
#include "mainwindow.h"

ControlCenterWidget::ControlCenterWidget(QWidget *parent) : QWidget(parent)
{
    m_modified = false;
}

void ControlCenterWidget::setModified(bool m)
{
    m_modified = m;

    emit modified(m);
}

bool ControlCenterWidget::isModified()
{
    return m_modified;
}
