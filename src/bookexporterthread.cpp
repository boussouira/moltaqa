#include "bookexporterthread.h"
#include "richsimplebookreader.h"
#include "richtafessirreader.h"
#include "richquranreader.h"
#include "librarymanager.h"
#include "librarybookmanager.h"
#include "bookexporter.h"
#include "utils.h"
#include <qdebug.h>

BookExporterThread::BookExporterThread(QObject *parent) :
    QThread(parent)
{
    m_stop = false;
}

void BookExporterThread::run()
{
    LibraryBookManager *manager = LibraryManager::instance()->bookManager();

    foreach (int bookID, m_bookToImport) {
        LibraryBookPtr book = manager->getLibraryBook(bookID);

        try {
            importBook(book);

            emit bookExported(book->title);
        } catch (BookException &e) {
            e.print();
        }

        if(m_stop) {
            m_stop = false;
            break;
        }
    }

    emit doneExporting();
}

void BookExporterThread::stop()
{
    m_stop = true;
}

void BookExporterThread::importBook(LibraryBookPtr book)
{
    BookExporter exporter;
    exporter.setLibraryBook(book);
    exporter.start();

    QDir dir(m_outDir);
    QString filePath = Utils::Files::cleanFileName(dir.filePath(book->title + ".mlp"));
    filePath = Utils::Files::ensureFileExistsNot(filePath);

    if(!Utils::Files::move(exporter.genereatedPath(), filePath)) {
        QFile::remove(exporter.genereatedPath());

        throw BookException(QString("BookExporterThread::importBook can't move file %1 to %2")
                            .arg(exporter.genereatedPath())
                            .arg(filePath));
    }
}
