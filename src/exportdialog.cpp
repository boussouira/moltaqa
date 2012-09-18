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
    addPage(new ExportPage(this));

    connect(this, SIGNAL(currentIdChanged(int)), SLOT(pageChanged(int)));
}

void ExportDialog::pageChanged(int pageID)
{
    if(pageID == 2) {
        button(QWizard::CancelButton)->disconnect();
        button(QWizard::BackButton)->disconnect();
        button(QWizard::BackButton)->setEnabled(false);

        connect(button(QWizard::CancelButton), SIGNAL(clicked()), SLOT(cancelExport()));
    }
}

void ExportDialog::cancelExport()
{
    ExportPage *p = qobject_cast<ExportPage*>(page(2));
    ml_return_on_fail(p);

    p->exportThread()->stop();
}
