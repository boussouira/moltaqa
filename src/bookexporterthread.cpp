#include "bookexporterthread.h"
#include "richsimplebookreader.h"
#include "richtafessirreader.h"
#include "richquranreader.h"
#include "librarymanager.h"
#include "librarybookmanager.h"
#include "bookexporter.h"
#include "librarybookexporter.h"
#include "epubbookexporter.h"
#include "utils.h"
#include <qdebug.h>

BookExporterThread::BookExporterThread(QObject *parent) :
    QThread(parent)
{
    m_exportFormat = MOLTAQA_FROMAT;
    m_removeTashkil = false;
    m_addPageNumber = false;
    m_stop = false;
}

void BookExporterThread::run()
{
    LibraryBookManager *manager = LibraryManager::instance()->bookManager();

    foreach (int bookID, m_bookToImport) {
        LibraryBookPtr book = manager->getLibraryBook(bookID);

        try {
            exportBook(book);

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

void BookExporterThread::setBooksToExport(QList<int> list)
{
    m_bookToImport = list;
}

QList<int> BookExporterThread::booksToExport()
{
    return m_bookToImport;
}

void BookExporterThread::setRemoveTashkil(bool remove)
{
    m_removeTashkil = remove;
}

void BookExporterThread::setAddPageNumber(bool add)
{
    m_addPageNumber = add;
}

void BookExporterThread::setExportFormat(ExportFormat format)
{
    m_exportFormat = format;
}

void BookExporterThread::setOutDir(const QString &dir)
{
    m_outDir = dir;
}

void BookExporterThread::stop()
{
    m_stop = true;
}

void BookExporterThread::exportBook(LibraryBookPtr book)
{
    BookExporter *exporter = 0;
    if(m_exportFormat == MOLTAQA_FROMAT) {
        exporter = new LibraryBookExporter();
    } else if(m_exportFormat == EPUB_FROMAT) {
        exporter = new EPubBookExporter();
    } else {
        throw BookException(QString("BookExporterThread::importBook Unknow format %1").arg(m_exportFormat));
    }

    exporter->setLibraryBook(book);
    exporter->setAddPageNumber(m_addPageNumber);
    exporter->setRemoveTashkil(m_removeTashkil);
    exporter->start();

    QDir dir(m_outDir);
    QString filePath = dir.filePath(book->title + '.' +
                                    QFileInfo(exporter->genereatedPath()).completeSuffix());

    filePath = Utils::Files::cleanFileName(filePath);
    filePath = Utils::Files::ensureFileExistsNot(filePath);

    if(!Utils::Files::move(exporter->genereatedPath(), filePath)) {
        QFile::remove(exporter->genereatedPath());

        throw BookException(QString("BookExporterThread::importBook can't move file %1 to %2")
                            .arg(exporter->genereatedPath())
                            .arg(filePath));
    }
}
