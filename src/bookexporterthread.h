#ifndef BOOKEXPORTERTHREAD_H
#define BOOKEXPORTERTHREAD_H

#include "exportformats.h"
#include "librarybook.h"

#include <qthread.h>

class BookExporter;

class BookExporterThread : public QThread
{
    Q_OBJECT
public:
    BookExporterThread(QObject *parent = 0);

    void run();

    void setBooksToExport(QList<int> list);
    QList<int> booksToExport();

    void setExportInOnePackage(bool onePackage);
    void setRemoveTashkil(bool remove);
    void setAddPageNumber(bool add);
    void setExportFormat(ExportFormat format);

    void setOutDir(const QString &dir);

public slots:
    void stop();

protected:
    void moveToOutDir(QString filePath, QString fileName);

signals:
    void doneExporting();
    void bookExported(QString book);

protected:
    QList<int> m_bookToImport;
    QString m_outDir;
    BookExporter *m_exporter;
    ExportFormat m_exportFormat;
    bool m_stop;
    bool m_exportInOnePackage;
    bool m_removeTashkil;
    bool m_addPageNumber;
};

#endif // BOOKEXPORTERTHREAD_H
