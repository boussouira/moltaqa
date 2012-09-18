#ifndef EXPORTDIALOGPAGES_H
#define EXPORTDIALOGPAGES_H

#include <qwizard.h>
#include "bookexporterthread.h"

class QCheckBox;

class IntroPage : public QWizardPage
{
    Q_OBJECT
public:
    IntroPage(QWidget *parent = 0);

protected slots:
    void formatChanged(int index);

protected:
    QCheckBox *m_checkRemoveTashkil;
    QCheckBox *m_checkAddPageNumber;
};

class BookSelectionPage : public QWizardPage
{
public:
    BookSelectionPage(QWidget *parent = 0);

    bool validatePage();

protected:
    class LibrarySearchFilter *m_filter;
};

class ExportPage : public QWizardPage
{
    Q_OBJECT

public:
    ExportPage(QWidget *parent = 0);

    void initializePage();
    bool isComplete() const;

    BookExporterThread *exportThread() { return m_thread; }

protected slots:
    void bookExported(QString book);
    void doneExporting();

protected:
    class QLabel *m_label;
    class QListWidget *m_treeWidget;
    class QProgressBar *m_progressBar;
    BookExporterThread *m_thread;
    bool m_doneExport;
};


#endif // EXPORTDIALOGPAGES_H
