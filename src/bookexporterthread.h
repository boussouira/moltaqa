#ifndef BOOKEXPORTERTHREAD_H
#define BOOKEXPORTERTHREAD_H

#include <qthread.h>
#include "librarybook.h"
#include "exportformats.h"

class BookExporterThread : public QThread
{
    Q_OBJECT
public:
    BookExporterThread(QObject *parent = 0);

    void run();

    void setBooksToImport(QList<int> list);
    QList<int> booksToImport();

    void setRemoveTashkil(bool remove);
    void setAddPageNumber(bool add);
    void setExportFormat(ExportFormat format);

    void setOutDir(const QString &dir);

public slots:
    void stop();

protected:
    void importBook(LibraryBookPtr book);

signals:
    void doneExporting();
    void bookExported(QString book);

protected:
    QList<int> m_bookToImport;
    QString m_outDir;
    ExportFormat m_exportFormat;
    bool m_stop;
    bool m_removeTashkil;
    bool m_addPageNumber;
};

#endif // BOOKEXPORTERTHREAD_H
