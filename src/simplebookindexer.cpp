#include "simplebookindexer.h"
#include "clheader.h"
#include "clutils.h"
#include "clconstants.h"
#include "librarymanager.h"
#include "authorsmanager.h"
#include "bookexception.h"
#include "bookutils.h"
#include "utils.h"

#include <qdebug.h>
#include <qxmlstream.h>
#include <qfile.h>

BookIndexerSimple::BookIndexerSimple() :
    m_authorDeath(0),
    m_unknowAuthor(false),
    m_lastTitleID(-1)
{

}

BookIndexerSimple::~BookIndexerSimple()
{
    if(m_authorDeath)
        free(m_authorDeath);
}

void BookIndexerSimple::open()
{
    if(!m_book)
        throw BookException("BookIndexerQuran::open book is null");

    if(!QFile::exists(m_book->path))
               throw BookException("BookIndexerQuran::open file doesn't exists", m_book->path);

    m_zip.setZipName(m_book->path);

    if(!m_zip.open(QuaZip::mdUnzip))
        throw BookException("BookIndexerQuran::open Can't zip file",
                                m_book->path, m_zip.getZipError());
}

void BookIndexerSimple::start()
{
    loadTitles();

    BookPage page;
    QuaZipFileInfo info;
    QuaZipFile file(&m_zip);
    for(bool more=m_zip.goToFirstFile(); more; more=m_zip.goToNextFile()) {
        ml_return_on_fail2(m_zip.getCurrentFileInfo(&info),
                            "BookIndexerSimple::start getCurrentFileInfo Error" << m_zip.getZipError());

        int id = 0;
        QString name = info.name;
        if(name.startsWith("pages/p")) {
            name = name.remove(0, 7);
            name = name.remove(".html");

            bool ok;
            id = name.toInt(&ok);
            if(!ok) {
                qDebug("BookIndexerSimple::start can't convert '%s' to int", qPrintable(name));
                continue;
            }
        } else {
            continue;
        }

        if(!file.open(QIODevice::ReadOnly)) {
            qWarning("BookIndexerSimple::start zip error %d", m_zip.getZipError());
            continue;
        }

        QByteArray out;
        char buf[4096];
        int len = 0;

        while (!file.atEnd()) {
            len = file.read(buf, 4096);
            out.append(buf, len);

            if(len <= 0)
                break;
        }

        page.pageID = id;
        page.text = QString::fromUtf8(out);

        page.titleID = Utils::Book::getPageTitleID(m_titles, page.pageID);

        if(m_lastTitleID != page.titleID) {
            m_lastTitleID = page.titleID;
            page.title = m_titlesText[m_lastTitleID];
        } else {
            page.title.clear();
        }

        addPageToIndex(&page);

        file.close();

        if(file.getZipError()!=UNZ_OK) {
            qWarning("BookIndexerSimple::start Unknow zip error %d", file.getZipError());
            continue;
        }
    }
}

void BookIndexerSimple::loadTitles()
{
    QuaZipFile titleFile(&m_zip);

    if(!m_zip.setCurrentFile("titles.xml"))
        throw BookException("BookIndexerSimple::loadTitles setCurrentFile error", titleFile.getZipError());

    if(!titleFile.open(QIODevice::ReadOnly))
        throw BookException("BookIndexerSimple::loadTitles open error", titleFile.getZipError());

    QXmlStreamReader reader(&titleFile);
    int titleID = -1;

    while(!reader.atEnd()) {
        reader.readNext();

        if(reader.isStartElement()) {
            if(reader.name() == "title") {
                titleID = reader.attributes().value("pageID").toString().toInt();
                m_titles.append(titleID);
            } else if(!m_book->isTafessir() && reader.name() == "text") {
                if(reader.readNext() == QXmlStreamReader::Characters) {
                    if(m_titlesText.contains(titleID))
                        m_titlesText[titleID].append(' ').append(reader.text().toString());
                    else
                        m_titlesText[titleID] = reader.text().toString();
                } else {
                    if(reader.tokenType() != QXmlStreamReader::EndElement) { // Ignore empty titles
                        qWarning() << "TextBookReader::getTitles Unexpected token type" << reader.tokenString()
                                   << "- Book:" << m_book->id << m_book->title << m_book->fileName;
                        break;
                    }
                }
            }
        }

        if(reader.hasError()) {
            qDebug() << "BookIndexerSimple::loadTitles QXmlStreamReader error:" << reader.errorString()
                     << "- Book:" << m_book->id << m_book->title << m_book->fileName;
            break;
        }
    }


    qSort(m_titles);

    titleFile.close();
}

void BookIndexerSimple::morePageIndex(BookPage *page)
{
    if(!m_authorDeath && !m_unknowAuthor) {
        AuthorInfo::Ptr author = LibraryManager::instance()->authorsManager()->getAuthorInfo(m_book->authorID);
        if(author) {
            int death = (author->isALive ? 9999 : (author->unknowDeath ? 0 : author->deathYear));
            m_authorDeath = Utils::CLucene::intToWChar(death);
        } else {
            m_unknowAuthor = true;
        }
    }

    m_doc->add( *_CLNEW Field(TITLE_ID_FIELD,
                              Utils::CLucene::intToWChar(page->titleID),
                              m_storeAndNoToken, false));

    if(m_authorDeath) {
        m_doc->add( *_CLNEW Field(AUTHOR_DEATH_FIELD,
                                  m_authorDeath,
                                  m_storeAndNoToken));
    }

    if(page->title.size()) {
        m_doc->add( *_CLNEW Field(TITLE_TEXT_FIELD,
                                  Utils::CLucene::QStringToWChar(page->title),
                                  m_tokenAndNoStore, false));
    }
}
