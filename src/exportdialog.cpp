#include "exportdialog.h"
#include "exportdialogpages.h"
#include "utils.h"
#include <qabstractbutton.h>

ExportDialog::ExportDialog(QWidget *parent) :
    QWizard(parent)
{
    setWindowTitle(tr("تصدير الكتب"));

    addPage(new IntroPage(this));
    addPage(new BookSelectionPage(this));

    m_exportPage = new ExportPage(this);
    addPage(m_exportPage);

    connect(this, SIGNAL(currentIdChanged(int)), SLOT(pageChanged(int)));
}

void ExportDialog::pageChanged(int pageID)
{
    if(pageID == 2) {
        button(QWizard::CancelButton)->disconnect();
        button(QWizard::BackButton)->disconnect();
        button(QWizard::BackButton)->setEnabled(false);

        connect(button(QWizard::CancelButton), SIGNAL(clicked()), SLOT(cancelExport()));
        connect(button(QWizard::FinishButton), SIGNAL(clicked()), m_exportPage, SLOT(openOutDir()));
    }
}

void ExportDialog::cancelExport()
{
    m_exportPage->exportThread()->stop();
}

