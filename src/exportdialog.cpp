#include "exportdialog.h"
#include "exportdialogpages.h"
#include "utils.h"
#include <qabstractbutton.h>

ExportDialog::ExportDialog(QWidget *parent) :
    QWizard(parent)
{
    setWindowTitle(tr("تصدير الكتب"));
    setOption(QWizard::NoBackButtonOnStartPage);
    setOption(QWizard::NoBackButtonOnLastPage);


    addPage(new IntroPage(this));
    addPage(new BookSelectionPage(this));
    addPage(new ExportPage(this));
}


