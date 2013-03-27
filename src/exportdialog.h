#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <qwizard.h>

class ExportPage;

class ExportDialog : public QWizard
{
    Q_OBJECT
public:
    ExportDialog(QWidget *parent = 0);

    void setBooksToImport(QList<int> list) { m_bookToImport = list; }
    QList<int> booksToImport() { return m_bookToImport; }

protected:
    QList<int> m_bookToImport;
};

#endif // EXPORTDIALOG_H
