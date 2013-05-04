#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <qwizard.h>

class ExportPage;

class ExportDialog : public QWizard
{
    Q_OBJECT
public:
    ExportDialog(QWidget *parent = 0);

    void setBooksToImport(QList<int> list);
    QList<int> booksToImport();

protected:
    QList<int> m_bookToImport;
};

#endif // EXPORTDIALOG_H
