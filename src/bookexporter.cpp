#include "bookexporter.h"
#include "utils.h"
#include "librarymanager.h"
#include "libraryinfo.h"
#include "ziphelper.h"
#include "xmldomhelper.h"
#include "authorsmanager.h"

BookExporter::BookExporter()
{
    m_tempDir = LibraryManager::instance()->libraryInfo()->tempDir();
}

void BookExporter::start()
{
    openZip();

    addBookInfo();
    addAuthorInfo();
    addBookFile();

    closeZip();
}

void BookExporter::openZip()
{
    QString bookPath = Utils::Rand::fileName(m_tempDir, "true", "book_", "mlp");

    while(QFile::exists(bookPath)) {
        bookPath.replace(".mlp", "_.mlp");
    }

    m_zip.setZipName(bookPath);

    if(!m_zip.open(QuaZip::mdCreate))
        throw BookException("BookExporter::openZip Can't creat zip file", bookPath, m_zip.getZipError());


    m_genereatedPath = bookPath;
}

void BookExporter::closeZip()
{
    m_zip.close();

    if(m_zip.getZipError()!=0)
        throw BookException("BookExporter::closeZip close zip", m_zip.getZipError());


    deleteTemp();
}

void BookExporter::addBookInfo()
{
    XmlDomHelper dom;
    dom.setFilePath(Utils::Rand::fileName(m_tempDir, "true", "info_", "xml"));
    dom.setDocumentName("books-info");

    dom.create();
    dom.load();

    QDomElement bookElement = dom.domDocument().createElement("book");
    bookElement.setAttribute("id", m_book->id);
    bookElement.setAttribute("type", m_book->type);
    bookElement.setAttribute("author", m_book->authorID);
    bookElement.setAttribute("flags", m_book->bookFlags);

    dom.setElementText(bookElement, "title", m_book->title);

    if(m_book->otherTitles.size())
        dom.setElementText(bookElement, "otherTitles", m_book->otherTitles);

    if(m_book->edition.size())
        dom.setElementText(bookElement, "edition", m_book->edition);

    if(m_book->publisher.size())
        dom.setElementText(bookElement, "publisher", m_book->publisher);

    if(m_book->mohaqeq.size())
        dom.setElementText(bookElement, "mohaqeq", m_book->mohaqeq);

    if(m_book->comment.size())
        dom.setElementText(bookElement, "comment", m_book->comment, true);

    if(m_book->info.size())
        dom.setElementText(bookElement, "info", m_book->info, true);

    dom.rootElement().appendChild(bookElement);

    QuaZipFile booksFile(&m_zip);
    if(booksFile.open(QIODevice::WriteOnly, QuaZipNewInfo("books.xml"))) {
        dom.save(&booksFile);

        booksFile.close();
    } else {
        throw BookException("BookExporter::addBookInfo error when writing to info.xml", booksFile.getZipError());
    }

    m_tempFiles << dom.filePath();
}

void BookExporter::addAuthorInfo()
{
    XmlDomHelper dom;
    dom.setFilePath(Utils::Rand::fileName(m_tempDir, "true", "authors_", "xml"));
    dom.setDocumentName("authors-info");

    dom.create();
    dom.load();

    AuthorInfoPtr author = LibraryManager::instance()->authorsManager()->getAuthorInfo(m_book->authorID);
    if(author) {
        int flags = 0;
        if(author->unknowBirth)
            flags |= AuthorInfo::UnknowBirth;
        if(author->unknowDeath)
            flags |= AuthorInfo::UnknowDeath;
        if(author->isALive)
            flags |= AuthorInfo::ALive;

        QDomElement authorElement = dom.domDocument().createElement("author");
        authorElement.setAttribute("id", author->id);
        authorElement.setAttribute("flags", flags);

        dom.setElementText(authorElement, "name", author->name);
        dom.setElementText(authorElement, "full-name", author->fullName);
        dom.setElementText(authorElement, "info", author->info, true);

        dom.setElementText(authorElement, "birth", author->birthStr).setAttribute("year", author->birthYear);
        dom.setElementText(authorElement, "death", author->deathStr).setAttribute("year", author->deathYear);

        dom.rootElement().appendChild(authorElement);
    }

    QuaZipFile authorFile(&m_zip);
    if(authorFile.open(QIODevice::WriteOnly, QuaZipNewInfo("authors.xml"))) {
        dom.save(&authorFile);

        authorFile.close();
    } else {
        throw BookException("BookExporter::addAuthorInfo error when writing to authors.xml",
                            authorFile.getZipError());
    }

    m_tempFiles << dom.filePath();
}

void BookExporter::addBookFile()
{
    QuaZipFile bookFile(&m_zip);
    if(!bookFile.open(QIODevice::WriteOnly, QuaZipNewInfo(m_book->fileName))) {
        throw BookException("BookExporter::addBookFile Can't add book file to the archive",
                            m_book->path,
                            bookFile.getZipError());
    }

    QFile inFile;
    inFile.setFileName(m_book->path);

    if(!inFile.open(QIODevice::ReadOnly)) {
        qWarning("BookExporter::addBookFile open input file error: %s", qPrintable(inFile.errorString()));
        return;
    }

    char buf[4096];
    qint64 l = 0;

    while (!inFile.atEnd()) {
         l = inFile.read(buf, 4096);
        if (l < 0) {
            qWarning("BookExporter::addBookFile input file read error: %s", qPrintable(inFile.errorString()));
            break;
        }
        if (l == 0)
            break;
        if (bookFile.write(buf, l) != l) {
            qWarning("BookExporter::addBookFile write chunk error: %d", bookFile.getZipError());
            break;
        }
    }

    bookFile.close();

    if(bookFile.getZipError()!=UNZ_OK) {
        throw BookException("BookExporter::addBookFile outFile close error",
                            bookFile.getZipError());
    }
}

void BookExporter::deleteTemp()
{
    foreach (QString filePath, m_tempFiles) {
        Utils::Files::removeDir(filePath);
        m_tempFiles.removeAll(filePath);
    }
}
