#include "exportdialog.h"
#include "exportdialogpages.h"
#include "utils.h"

#include <qabstractbutton.h>

ExportDialog::ExportDialog(QWidget *parent) :
    QWizard(parent)
{
    setWindowTitle(tr("تصدير الكتب"));
    resize(600, 400);
    setOption(QWizard::DisabledBackButtonOnLastPage);

    addPage(new IntroPage(this));
    addPage(new BookSelectionPage(this));
    addPage(new ExportPage(this));
}

void ExportDialog::setBooksToImport(QList<int> list)
{
    m_bookToImport = list;
}

QList<int> ExportDialog::booksToImport()
{
    return m_bookToImport;
}


