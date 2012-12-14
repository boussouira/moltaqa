#include "librarybookexporter.h"
#include "utils.h"
#include "librarymanager.h"
#include "booklistmanager.h"
#include "libraryinfo.h"
#include "ziphelper.h"
#include "authorsmanager.h"

LibraryBookExporter::LibraryBookExporter(QObject *parent) :
    BookExporter(parent)
{
}

void LibraryBookExporter::start()
{
    createZip();

    if(m_exportInOnePackage && m_bookList.size() > 1) {
        for(int i=0; i<m_bookList.size(); i++) {
            m_book = m_bookList[i];

            try {
                addBookInfo();
                addAuthorInfo();
                addBookFile();

                m_exportedBooks++;

                emit bookExported(m_book->title);

                if(m_stop) {
                    break;
                }

            } catch (BookException &e) {
                e.print();
            }
        }
    } else {
        if(!m_book) {
            if(m_bookList.size())
                m_book = m_bookList.first();
            else
                throw BookException("LibraryBookExporter::start no library book");
        }

        addBookInfo();
        addAuthorInfo();
        addBookFile();
    }

    closeZip();
}

void LibraryBookExporter::createZip()
{
    QString bookPath = Utils::Rand::fileName(m_tempDir, true, "book_", "mlp");

    while(QFile::exists(bookPath)) {
        bookPath.replace(".mlp", "_.mlp");
    }

    m_zip.setZipName(bookPath);

    if(!m_zip.open(QuaZip::mdCreate))
        throw BookException("LibraryBookExporter::openZip Can't creat zip file", bookPath, m_zip.getZipError());

    // Create content file
    m_contentDom.setFilePath(Utils::Rand::fileName(m_tempDir, true, "content_", "xml"));
    m_contentDom.setDocumentName("package-content");

    m_contentDom.create();
    m_contentDom.load();

    m_booksElement = m_contentDom.domDocument().createElement("books");
    m_authorsElement = m_contentDom.domDocument().createElement("authors");

    m_genereatedPath = bookPath;
    m_tempFiles << m_contentDom.filePath();
}

void LibraryBookExporter::closeZip()
{

    m_contentDom.rootElement().appendChild(m_booksElement);
    m_contentDom.rootElement().appendChild(m_authorsElement);

    QuaZipFile contentFile(&m_zip);
    if(contentFile.open(QIODevice::WriteOnly, QuaZipNewInfo("content.xml"))) {
        m_contentDom.save(&contentFile);

        contentFile.close();
    } else {
        throw BookException("LibraryBookExporter::closeZip error when writing to content.xml",
                            contentFile.getZipError());
    }

    m_zip.close();

    if(m_zip.getZipError()!=0)
        throw BookException("LibraryBookExporter::closeZip close zip", m_zip.getZipError());


    deleteTemp();
}

void LibraryBookExporter::addBookInfo()
{
    QDomElement bookElement = m_contentDom.domDocument().createElement("book");
    bookElement.setAttribute("id", m_book->id);
    bookElement.setAttribute("type", m_book->type);
    bookElement.setAttribute("author", m_book->authorID);
    bookElement.setAttribute("flags", m_book->bookFlags);

    m_contentDom.setElementText(bookElement, "fileName", m_book->fileName);

    m_contentDom.setElementText(bookElement, "title", m_book->title);

    QList<CategorieInfo> books = LibraryManager::instance()->bookListManager()->bookCategorie(m_book->id);

    if(books.size()) {
        QDomElement catsElement = m_contentDom.domDocument().createElement("categories");

        foreach(const CategorieInfo &cat, books) {
            m_contentDom.setElementText(catsElement, "cat", cat.title).setAttribute("id", cat.catID);
        }

        bookElement.appendChild(catsElement);
    }

    if(m_book->otherTitles.size())
        m_contentDom.setElementText(bookElement, "otherTitles", m_book->otherTitles);

    if(m_book->edition.size())
        m_contentDom.setElementText(bookElement, "edition", m_book->edition);

    if(m_book->publisher.size())
        m_contentDom.setElementText(bookElement, "publisher", m_book->publisher);

    if(m_book->mohaqeq.size())
        m_contentDom.setElementText(bookElement, "mohaqeq", m_book->mohaqeq);

    if(m_book->comment.size())
        m_contentDom.setElementText(bookElement, "comment", m_book->comment, true);

    if(m_book->info.size())
        m_contentDom.setElementText(bookElement, "info", m_book->info, true);

    m_booksElement.appendChild(bookElement);
}

void LibraryBookExporter::addAuthorInfo()
{
    if(m_addedAuthorsInfo.contains(m_book->authorID))
        return;

    AuthorInfoPtr author = LibraryManager::instance()->authorsManager()->getAuthorInfo(m_book->authorID);
    if(author) {
        int flags = 0;
        if(author->unknowBirth)
            flags |= AuthorInfo::UnknowBirth;
        if(author->unknowDeath)
            flags |= AuthorInfo::UnknowDeath;
        if(author->isALive)
            flags |= AuthorInfo::ALive;

        QDomElement authorElement = m_contentDom.domDocument().createElement("author");
        authorElement.setAttribute("id", author->id);
        authorElement.setAttribute("flags", flags);

        m_contentDom.setElementText(authorElement, "name", author->name);
        m_contentDom.setElementText(authorElement, "full-name", author->fullName);
        m_contentDom.setElementText(authorElement, "info", author->info, true);

        m_contentDom.setElementText(authorElement, "birth", author->birthStr).setAttribute("year", author->birthYear);
        m_contentDom.setElementText(authorElement, "death", author->deathStr).setAttribute("year", author->deathYear);

        m_authorsElement.appendChild(authorElement);
        m_addedAuthorsInfo.append(m_book->authorID);
    }
}

void LibraryBookExporter::addBookFile()
{
    QuaZipFile bookFile(&m_zip);
    if(!bookFile.open(QIODevice::WriteOnly, QuaZipNewInfo(m_book->fileName))) {
        throw BookException("LibraryBookExporter::addBookFile Can't add book file to the archive",
                            m_book->path,
                            bookFile.getZipError());
    }

    QFile inFile;
    inFile.setFileName(m_book->path);

    if(!inFile.open(QIODevice::ReadOnly)) {
        qWarning("LibraryBookExporter::addBookFile open input file error: %s", qPrintable(inFile.errorString()));
        return;
    }

    char buf[4096];
    qint64 l = 0;

    while (!inFile.atEnd()) {
         l = inFile.read(buf, 4096);
        if (l < 0) {
            qWarning("LibraryBookExporter::addBookFile input file read error: %s", qPrintable(inFile.errorString()));
            break;
        }
        if (l == 0)
            break;
        if (bookFile.write(buf, l) != l) {
            qWarning("LibraryBookExporter::addBookFile write chunk error: %d", bookFile.getZipError());
            break;
        }
    }

    bookFile.close();

    if(bookFile.getZipError()!=UNZ_OK) {
        throw BookException("LibraryBookExporter::addBookFile outFile close error",
                            bookFile.getZipError());
    }
}

void LibraryBookExporter::deleteTemp()
{
    foreach (QString filePath, m_tempFiles) {
        Utils::Files::removeDir(filePath);
        m_tempFiles.removeAll(filePath);
    }
}
