#include "wizardpage.h"

WizardPage::WizardPage(QWidget *parent) :
    QWizardPage(parent)
{
    m_complete = true;
}

void WizardPage::setComplete(bool complete)
{
    if(m_complete == complete)
        return;

    m_complete = complete;

    emit completeChanged();
}

bool WizardPage::isComplete() const
{
    return m_complete;
}
