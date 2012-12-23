#include "bookexporterthread.h"
#include "richsimplebookreader.h"
#include "richtafessirreader.h"
#include "richquranreader.h"
#include "librarymanager.h"
#include "librarybookmanager.h"
#include "bookexporter.h"
#include "librarybookexporter.h"
#include "epubbookexporter.h"
#include "htmlbookexporter.h"
#include "utils.h"
#include <qdebug.h>

BookExporterThread::BookExporterThread(QObject *parent) :
    QThread(parent)
{
    m_exporter = 0;
    m_exportFormat = MOLTAQA_FROMAT;
    m_exportInOnePackage = false;
    m_removeTashkil = false;
    m_addPageNumber = false;
    m_stop = false;
}

void BookExporterThread::run()
{
    LibraryBookManager *manager = LibraryManager::instance()->bookManager();

    try {
        if(m_exportFormat == MOLTAQA_FROMAT) {
            m_exporter = new LibraryBookExporter();
        } else if(m_exportFormat == EPUB_FROMAT) {
            m_exporter = new EPubBookExporter();
        } else if(m_exportFormat == HTML_FORMAT) {
            m_exporter = new HtmlBookExporter();
        } else {
            throw BookException(QString("BookExporterThread::importBook Unknow format %1").arg(m_exportFormat));
        }
    } catch (BookException &e) {
        e.print();
        return;
    }

    m_exporter->setOutDir(m_outDir);
    m_exporter->setAddPageNumber(m_addPageNumber);
    m_exporter->setRemoveTashkil(m_removeTashkil);

    if(m_exportInOnePackage
            && m_exporter->multiBookExport()
            && m_bookToImport.size() > 1) {
        QList<LibraryBook::Ptr> libraryBooks;
        foreach (int bookID, m_bookToImport) {
            LibraryBook::Ptr book = manager->getLibraryBook(bookID);
            if(book)
                libraryBooks.append(book);
        }

        connect(m_exporter,
                SIGNAL(bookExported(QString)),
                SIGNAL(bookExported(QString)));

        m_exporter->setLibraryBookList(libraryBooks);
        m_exporter->setExportInOnePackage(true);
        m_exporter->start();

        if(m_exporter->moveGeneratedFile()) {
            moveToOutDir(m_exporter->genereatedPath(),
                         tr("مجموعة كتب (%1)").arg(m_exporter->exportedBooksCount()));
        }
    } else {
        foreach (int bookID, m_bookToImport) {
            LibraryBook::Ptr book = manager->getLibraryBook(bookID);

            try {
                m_exporter->setLibraryBook(book);
                m_exporter->start();

                if(m_exporter->moveGeneratedFile())
                    moveToOutDir(m_exporter->genereatedPath(), book->title);

                emit bookExported(book->title);
            } catch (BookException &e) {
                e.print();
            }

            if(m_stop) {
                m_stop = false;
                break;
            }
        }
    }

    ml_delete_check(m_exporter);

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

void BookExporterThread::setExportInOnePackage(bool onePackage)
{
    m_exportInOnePackage = onePackage;
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

    if(m_exporter)
        m_exporter->stop();
}

void BookExporterThread::moveToOutDir(QString filePath, QString fileName)
{
    QDir dir(m_outDir);
    QString newFilePath = dir.filePath(fileName + '.' +
                                    QFileInfo(filePath).completeSuffix());

    newFilePath = Utils::Files::cleanFileName(newFilePath);
    newFilePath = Utils::Files::ensureFileExistsNot(newFilePath);

    if(!Utils::Files::move(filePath, newFilePath)) {
        QFile::remove(filePath);

        throw BookException(QString("BookExporterThread::importBook can't move file %1 to %2")
                            .arg(filePath)
                            .arg(newFilePath));
    }
}
