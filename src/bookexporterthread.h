#ifndef BOOKEXPORTERTHREAD_H
#define BOOKEXPORTERTHREAD_H

#include <qthread.h>
#include "librarybook.h"

class BookExporterThread : public QThread
{
    Q_OBJECT
public:
    BookExporterThread(QObject *parent = 0);

    void run();

    void setBooksToImport(QList<int> list) { m_bookToImport = list; }
    QList<int> booksToImport() { return m_bookToImport; }

    void setOutDir(const QString &dir) { m_outDir = dir; }

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
    bool m_stop;
};

#endif // BOOKEXPORTERTHREAD_H
