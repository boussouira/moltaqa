#include "bookeditor.h"
#include "richbookreader.h"
#include "librarybook.h"
#include "bookpage.h"
#include "utils.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "librarybookmanager.h"

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#include "richquranreader.h"
#include "richsimplebookreader.h"
#include "richtafessirreader.h"
#include "bookexception.h"

BookEditor::BookEditor(QObject *parent) :
    QObject(parent)
{
    m_lastBookID = 0;
    m_bookReader = 0;
    m_removeReader = false;
}

BookEditor::~BookEditor()
{
    if(m_removeReader) {
        ml_delete_check(m_bookReader);
    }

    removeTemp();
}

bool BookEditor::open(int bookID)
{
    return open(LibraryManager::instance()->bookManager()->getLibraryBook(bookID));
}

bool BookEditor::open(LibraryBookPtr book)
{
    if(book) {
        RichBookReader *bookReader;
        if(book->isNormal()) {
            bookReader = new RichSimpleBookReader();
        } else if(book->isTafessir()) {
            bookReader = new RichTafessirReader();
        } else {
            qDebug() << "BookEditor::open Can't edit book:" << book->path;
            return false;
        }

        bookReader->setBookInfo(book);

        try {
            bookReader->openBook();

            m_bookReader = bookReader;
            m_book = book;

            if(m_book->id != m_lastBookID)
                m_bookTmpDir.clear();

            m_removeReader = true;
            return true;

        } catch (BookException &) {
            delete bookReader;
            return false;
        }
    } else {
        return false;
    }
}

void BookEditor::setBookReader(RichBookReader *reader)
{
    m_bookReader = reader;
    m_book = reader->bookInfo();

    if(m_book->id != m_lastBookID) {
        removeTemp();
        m_bookTmpDir.clear();
    }

    m_removeReader = false;
}

void BookEditor::unZip()
{
    ml_return_on_fail(m_bookTmpDir.isEmpty());
    ml_return_on_fail(!QFile::exists(m_bookTmpDir));

    QString folder = QFileInfo(m_book->path).baseName();
    QDir dir(MW->libraryInfo()->tempDir());

    while(dir.exists(folder))
        folder.append("_");

    dir.mkdir(folder);
    dir.cd(folder);

    m_createdDirs.clear();

    QFile zipFile(m_book->path);
    QuaZip zip(&zipFile);

    if(!zip.open(QuaZip::mdUnzip)) {
        qWarning("BookEditor::unZip cant Open zip file %d", zip.getZipError());
    }

    QuaZipFileInfo info;
    QuaZipFile file(&zip);

    for(bool more=zip.goToFirstFile(); more; more=zip.goToNextFile()) {
        if(!zip.getCurrentFileInfo(&info)) {
            qWarning("BookEditor::unZip getCurrentFileInfo Error %d", zip.getZipError());
            continue;
        }

        QString outPath = dir.filePath(info.name);
        QFileInfo fileInfo(outPath);
        if(!m_createdDirs.contains(fileInfo.path())
                && dir.mkpath(fileInfo.path()))
            m_createdDirs.append(fileInfo.path());

        if(fileInfo.isDir())
            continue;

        if(!file.open(QIODevice::ReadOnly)) {
            qWarning("BookEditor::unZip open reader Error %d", zip.getZipError());
            continue;
        }

        QFile out(dir.filePath(info.name));
        if(!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qWarning() << "BookEditor::unZip open" << out.fileName()
                       << "for writing - Error" << out.errorString();
            file.close();
            continue;
        }

        char buf[4096];
        int len = 0;

        while (!file.atEnd()) {
            len = file.read(buf, 4096);
            out.write(buf, len);

            if(len <= 0)
                break;
        }

        out.close();
        file.close();
    }

    m_bookTmpDir = dir.path();
    m_lastBookID = m_book->id;
}

bool BookEditor::zip()
{
    QFile zipFile(QString("%1/%2.zip")
                  .arg(MW->libraryInfo()->tempDir())
                  .arg(QFileInfo(m_book->path).baseName()));

    QuaZip zip(&zipFile);
    if(!zip.open(QuaZip::mdCreate)) {
        qWarning("BookEditor::zip open zip error %d", zip.getZipError());
        return false;
    }

    QuaZipFile outFile(&zip);

    if(!zipDir(m_bookTmpDir, &outFile)) {
        zip.close();
        return false;
    }

    zip.close();

    if(zip.getZipError()!=0) {
        qWarning("BookEditor::zip close zip error %d", zip.getZipError());
        return false;
    }

    m_newBookPath = zipFile.fileName();

    return true;
}

bool BookEditor::save()
{
    // Delete Existing backup
    QString backupFile = m_book->path + ".back";

    if(QFile::exists(backupFile))
        QFile::remove(backupFile);

    if(m_removeReader) {
        ml_delete_check(m_bookReader);
    }

    // Create a new backup
    if(QFile::copy(m_book->path, backupFile)) {
        QFile::remove(m_book->path);
    } else {
        qWarning() << "BookEditor::save Can't make a backup for:" << m_book->path;
        return false;
    }

    // Copy new book
    if(QFile::copy(m_newBookPath, m_book->path)) {
        QFile::remove(m_newBookPath);
        m_newBookPath.clear();
    } else {
        qWarning() << "BookEditor::save Can't copy" << m_newBookPath << "to" << m_book->path;
        return false;
    }

    return true;
}

void BookEditor::removeTemp()
{
    if(m_bookTmpDir.size() && QFile::exists(m_bookTmpDir))
        Utils::Files::removeDir(m_bookTmpDir);
}

bool BookEditor::saveBookPages(QList<BookPage*> pages)
{
    foreach(BookPage *page, pages) {
        QString pagePath = QString("%1/pages/p%2.html").arg(m_bookTmpDir).arg(page->pageID);
        if(m_removedPages.contains(page->pageID)) {
            if(QFile::exists(pagePath))
                QFile::remove(pagePath);

            m_removedPages.removeAll(page->pageID);
            continue;
        }

        QFile file(pagePath);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qWarning() << "BookEditor::saveBookPages: Can't write page"
                       << page->pageID << "to:" << file.fileName();
            continue;
        }

        QDomElement e = m_bookReader->pagesDom().findElement("id", QString::number(page->pageID));
        if(!e.isNull()) {
            e.setAttribute("page", page->page);
            e.setAttribute("part", page->part);

            if(page->haddit)
                e.setAttribute("haddit", page->haddit);

            if(!m_book->isNormal()) {
                e.setAttribute("sora", page->sora);
                e.setAttribute("aya", page->aya);
            }
        } else {
            qDebug("BookEditor::saveBookPages no element with id %d", page->pageID);
        }

        QTextStream out(&file);
        out.setCodec("utf-8");

        out << page->text;
    }

    m_bookReader->pagesDom().save(QString("%1/pages.xml").arg(m_bookTmpDir));

    return true;
}

void BookEditor::saveDom()
{
    m_bookReader->pagesDom().save(QString("%1/pages.xml").arg(m_bookTmpDir));
}

void BookEditor::addPage(int pageID)
{
    QDomElement e = m_bookReader->pagesDom().currentElement();
    QDomElement page = m_bookReader->pagesDom().domDocument().createElement("item");

    page.setAttribute("id", pageID);
    page.setAttribute("page", e.attribute("page"));
    page.setAttribute("part", e.attribute("part"));

    if(m_book->isTafessir()) {
        page.setAttribute("aya", e.attribute("aya"));
        page.setAttribute("sora", e.attribute("sora"));
    }

    page.setAttribute("tid", m_bookReader->getPageTitleID(e.attribute("id").toInt()));

    QDomElement newPage = m_bookReader->pagesDom().rootElement().insertAfter(page, e).toElement();

    if(!newPage.isNull())
        m_bookReader->goToPage(pageID);
}

void BookEditor::removePage()
{
    QDomElement page = m_bookReader->pagesDom().currentElement();

    if(m_bookReader->hasNext())
        m_bookReader->nextPage();
    else
        m_bookReader->prevPage();

    QDomElement removedPage = m_bookReader->pagesDom().rootElement().removeChild(page).toElement();
    if(!removedPage.isNull())
        m_removedPages.append(removedPage.attribute("id").toInt());
}

bool BookEditor::zipDir(QString path, QuaZipFile *outFile)
{
    QFile inFile;
    QDir bookDir(path);
    QFileInfoList files = bookDir.entryInfoList(QDir::AllDirs|QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot);

    if(files.isEmpty()) {
        qWarning() << "BookEditor::zipDir Ziping an empty directory" << path;
    }

    QDir bookTempDir(m_bookTmpDir);

    foreach(QFileInfo file, files) {
        if(file.isDir()) {
            if(zipDir(file.filePath(), outFile))
                continue;
            else
                return false;
        }

        if(!file.isFile()) {
            qWarning() << "BookEditor::zipDir Can't zip file:" << file.filePath();
            continue;
        }

        QString inFilePath = bookTempDir.relativeFilePath(file.filePath());

        inFile.setFileName(file.filePath());

        if(!inFile.open(QIODevice::ReadOnly)) {
            qWarning("BookEditor::zipDir open input file error %s", qPrintable(inFile.errorString()));
            return false;
        }

        if(!outFile->open(QIODevice::WriteOnly, QuaZipNewInfo(inFilePath, inFilePath))) {
            qWarning("zip outFile->open(): %d", outFile->getZipError());
            return false;
        }

        char buf[4096];
        qint64 l = 0;

        while (!inFile.atEnd()) {
             l = inFile.read(buf, 4096);
            if (l < 0) {
                qWarning("BookEditor::zipDir read from input file error %s",
                         qPrintable(inFile.errorString()));
                break;
            }
            if (l == 0)
                break;
            if (outFile->write(buf, l) != l) {
                qWarning("BookEditor::zipDir write to output file error %d",
                         outFile->getZipError());
                break;
            }
        }

        if(outFile->getZipError()!=UNZ_OK) {
            qWarning("BookEditor::zipDir outFile error %d", outFile->getZipError());
            return false;
        }

        outFile->close();

        if(outFile->getZipError()!=UNZ_OK) {
            qWarning("BookEditor::zipDir outFile error %d", outFile->getZipError());
            return false;
        }

        inFile.close();
    }

    return true;
}

int BookEditor::maxPageID()
{
    int pageID = 0;
    QDomElement e = m_bookReader->pagesDom().rootElement().firstChildElement();
    while(!e.isNull()) {
        int pid = e.attribute("id").toInt();

        if(pid > pageID)
            pageID = pid;

        e = e.nextSiblingElement();
    }

    return pageID;
}

QString BookEditor::titlesFile()
{
    QDir dir(m_bookTmpDir);

    return dir.absoluteFilePath("titles.xml");
}

void BookEditor::addPageLink(int sourcPage, int destBook, int destPage)
{
    QDomElement pageElement = m_bookReader->pagesDom().findElement("id", sourcPage);
    if(!pageElement.isNull()) {
        QDomElement linkElement = m_bookReader->pagesDom().domDocument().createElement("link");
        linkElement.setAttribute("book", destBook);
        linkElement.setAttribute("page", destPage);

        pageElement.appendChild(linkElement);
    }
}

void BookEditor::removePageLink(int sourcPage, int destBook, int destPage)
{
    QDomElement pageElement = m_bookReader->pagesDom().findElement("id", sourcPage);
    if(!pageElement.isNull()) {
        QDomElement linkElement = pageElement.firstChildElement("link");
        while(!pageElement.isNull()) {
            if(linkElement.attribute("book").toInt() == destBook
                    && linkElement.attribute("page").toInt() == destPage) {
                pageElement.removeChild(linkElement);
                break;
            }

            linkElement = linkElement.nextSiblingElement("link");
        }
    }
}
