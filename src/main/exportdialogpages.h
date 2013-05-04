#ifndef EXPORTDIALOGPAGES_H
#define EXPORTDIALOGPAGES_H

#include <qwizard.h>

class BookExporterThread;
class QCheckBox;
class QLabel;
class QListWidget;
class QProgressBar;

class IntroPage : public QWizardPage
{
    Q_OBJECT
public:
    IntroPage(QWidget *parent = 0);

    bool validatePage();

protected slots:
    void formatChanged(int index);

protected:
    QCheckBox *m_checkRemoveTashkil;
    QCheckBox *m_checkAddPageNumber;
    QCheckBox *m_checkExportInOnePackage;
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

    BookExporterThread *exportThread();

public slots:
    void openOutDir();

protected slots:
    void bookExported(QString book);
    void doneExporting();

protected:
    QLabel *m_label;
    QListWidget *m_treeWidget;
    QProgressBar *m_progressBar;
    QCheckBox *m_checkOpenOutFolder;
    BookExporterThread *m_thread;
    bool m_doneExport;
};


#endif // EXPORTDIALOGPAGES_H
