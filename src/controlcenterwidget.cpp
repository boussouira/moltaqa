#include "controlcenterwidget.h"
#include "librarymanager.h"
#include "utils.h"

ControlCenterWidget::ControlCenterWidget(QWidget *parent) : QWidget(parent)
{
    m_modified = false;
    m_libraryManager = LibraryManager::instance();
}

void ControlCenterWidget::setModified(bool m)
{
    ML_ASSERT(m_modified != m);

    m_modified = m;

    emit modified(m);
}

bool ControlCenterWidget::isModified()
{
    return m_modified;
}

void ControlCenterWidget::aboutToShow()
{
}


void ControlCenterWidget::aboutToHide()
{
}
