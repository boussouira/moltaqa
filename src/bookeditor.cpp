#include "bookeditor.h"
#include "richbookreader.h"
#include "librarybook.h"
#include "bookpage.h"
#include "utils.h"
#include "mainwindow.h"
#include "libraryinfo.h"

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

BookEditor::BookEditor(QObject *parent) :
    QObject(parent)
{
    m_lastBookID = 0;
}

void BookEditor::setBookReader(RichBookReader *reader)
{
    m_bookReader = reader;
    m_book = reader->bookInfo();

    if(m_book->bookID != m_lastBookID)
        m_bookTmpDir.clear();
}

void BookEditor::unZip()
{
    QTime t;
    t.start();

    if(!m_bookTmpDir.isEmpty() && QFile::exists(m_bookTmpDir))
        return;

    QString folder = QFileInfo(m_book->bookPath).baseName();
    QDir dir(MW->libraryInfo()->tempDir());

    while(dir.exists(folder))
        folder.append("_");

    dir.mkdir(folder);
    dir.cd(folder);
    dir.mkdir("pages");

    QFile zipFile(m_book->bookPath);
    QuaZip zip(&zipFile);

    if(!zip.open(QuaZip::mdUnzip)) {
        qWarning("unZip: cant Open zip file %d", zip.getZipError());
    }

    QuaZipFileInfo info;
    QuaZipFile file(&zip);

    for(bool more=zip.goToFirstFile(); more; more=zip.goToNextFile()) {
        if(!zip.getCurrentFileInfo(&info)) {
            qWarning("getPages: getCurrentFileInfo Error %d", zip.getZipError());
            continue;
        }

        if(!file.open(QIODevice::ReadOnly)) {
            qWarning("unZip: open reader Error %d", zip.getZipError());
            continue;
        }

        QFile out(dir.filePath(info.name));
        if(!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qWarning("unZip: open writer Error %d", zip.getZipError());
            file.close();
            continue;
        }

        char buf[4096];
        int len = 0;

        while (!file.atEnd()) {
            len = file.read(buf, 4096);
            out.write(buf, len);
        }

        out.close();
        file.close();
    }

    qDebug("Unzip take %d ms", t.elapsed());
    m_bookTmpDir = dir.path();
    m_lastBookID = m_book->bookID;
}

bool BookEditor::zip()
{
    QTime t;
    t.start();

    QFile zipFile(QString("%1/%2.zip")
                  .arg(MW->libraryInfo()->tempDir())
                  .arg(QFileInfo(m_book->bookPath).baseName()));

    QuaZip zip(&zipFile);
    if(!zip.open(QuaZip::mdCreate)) {
        qWarning("zip zip.open(): %d", zip.getZipError());
        return false;
    }

    QuaZipFile outFile(&zip);

    if(!zipDir(m_bookTmpDir, &outFile)) {
        zip.close();
        return false;
    }

    zip.close();

    if(zip.getZipError()!=0) {
        qWarning("zip zip.close(): %d", zip.getZipError());
        return false;
    }

    m_newBookPath = zipFile.fileName();
    qDebug("Zip take %d ms", t.elapsed());

    return true;
}

bool BookEditor::save()
{
    // Delete Existing backup
    QString backupFile = m_book->bookPath + ".back";

    if(QFile::exists(backupFile))
        QFile::remove(backupFile);

    // Create a new backup
    if(QFile::copy(m_book->bookPath, backupFile)) {
        QFile::remove(m_book->bookPath);
    } else {
        qWarning() << "Can't make a backup for:" << m_book->bookPath;
        return false;
    }

    // Copy new book
    if(QFile::copy(m_newBookPath, m_book->bookPath)) {
        qDebug("Book saved");
        QFile::remove(m_newBookPath);
        m_newBookPath.clear();
    }

    return true;
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
            qWarning() << "saveBookPages: Can't write page" << page->pageID << "to:" << file.fileName();
            continue;
        }

        QDomElement e = m_bookReader->getPage(page->pageID);
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
            qDebug("NULL");
        }

        QTextStream out(&file);
        out.setCodec("utf-8");

        out << page->text;
    }

    QFile file(QString("%1/pages.xml").arg(m_bookTmpDir));
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "saveBookPages: Can't write pages.xml to:" << file.fileName();
        return false;
    }

    QTextStream out(&file);
    out.setCodec("utf-8");

    m_bookReader->m_bookDoc.save(out, 1);

    return true;
}

void BookEditor::addPage(int pageID)
{
    QDomElement e = m_bookReader->m_currentElement;
    QDomElement page = m_bookReader->m_bookDoc.createElement("item");

    page.setAttribute("id", pageID);
    page.setAttribute("page", e.attribute("page"));
    page.setAttribute("part", e.attribute("part"));

    if(m_book->isTafessir()) {
        page.setAttribute("aya", e.attribute("aya"));
        page.setAttribute("sora", e.attribute("sora"));
    }

    page.setAttribute("tid", m_bookReader->getPageTitleID(e.attribute("id").toInt()));

    QDomElement newPage = m_bookReader->m_rootElement.insertAfter(page, e).toElement();

    if(!newPage.isNull())
        m_bookReader->goToPage(pageID);
}

void BookEditor::removePage()
{
    QDomElement page = m_bookReader->m_currentElement;

    if(m_bookReader->hasNext())
        m_bookReader->nextPage();
    else
        m_bookReader->prevPage();

    QDomElement removedPage = m_bookReader->m_rootElement.removeChild(page).toElement();
    if(!removedPage.isNull())
        m_removedPages.append(removedPage.attribute("id").toInt());
}

bool BookEditor::zipDir(QString path, QuaZipFile *outFile)
{
    QFile inFile;
    QDir bookDir(path);
    QFileInfoList files = bookDir.entryInfoList(QDir::AllDirs|QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot);

    if(files.isEmpty()) {
        qWarning("Ziping an empty directory");
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
            qWarning() << "Can't zip file:" << file.filePath();
            continue;
        }

        QString inFilePath = bookTempDir.relativeFilePath(file.filePath());

        inFile.setFileName(file.filePath());

        if(!inFile.open(QIODevice::ReadOnly)) {
            qWarning("zip inFile.open(): %s", qPrintable(inFile.errorString()));
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
                qWarning("read(): %s", qPrintable(inFile.errorString()));
                break;
            }
            if (l == 0)
                break;
            if (outFile->write(buf, l) != l) {
                qWarning("write(): %d", outFile->getZipError());
                break;
            }
        }

        if(outFile->getZipError()!=UNZ_OK) {
            qWarning("zip outFile->putChar(): %d", outFile->getZipError());
            return false;
        }

        outFile->close();

        if(outFile->getZipError()!=UNZ_OK) {
            qWarning("zip outFile->close(): %d", outFile->getZipError());
            return false;
        }

        inFile.close();
    }

    return true;
}

int BookEditor::maxPageID()
{
    int pageID = 0;
    QDomElement e = m_bookReader->m_rootElement.firstChildElement();
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
