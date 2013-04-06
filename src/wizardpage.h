#ifndef WIZARDPAGE_H
#define WIZARDPAGE_H

#include <qwizard.h>

class WizardPage : public QWizardPage
{
    Q_OBJECT
    Q_PROPERTY(bool isComplete READ isComplete WRITE setComplete)

public:
    WizardPage(QWidget *parent = 0);
    
    void setComplete(bool complete);

    bool isComplete() const;

protected:
    bool m_complete;
};

#endif // WIZARDPAGE_H
