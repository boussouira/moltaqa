#include "richsimplebookreader.h"
#include "abstractbookreader.h"
#include "librarybook.h"
#include "bookexception.h"
#include "simpletextformat.h"
#include "libraryinfo.h"
#include "librarybookmanager.h"
#include "stringutils.h"

#include <qstringlist.h>
#include <qdebug.h>
#include <qdatetime.h>

RichSimpleBookReader::RichSimpleBookReader(QObject *parent) :
    RichBookReader(parent)
{
    m_textFormat = new SimpleTextFormat();
    m_showShorooh = true;
}

RichSimpleBookReader::~RichSimpleBookReader()
{
}

void RichSimpleBookReader::setShowShorooh(bool show)
{
    m_showShorooh = show;
}

void RichSimpleBookReader::connected()
{
    RichBookReader::connected();
}

void RichSimpleBookReader::setCurrentPage(QDomElement pageNode)
{
    m_textFormat->start();

    m_pagesDom.setCurrentElement(pageNode);

    m_currentPage->pageID = pageNode.attribute("id").toInt();
    m_currentPage->page = pageNode.attribute("page").toInt();
    m_currentPage->part = pageNode.attribute("part").toInt();
    m_currentPage->haddit = pageNode.attribute("haddit").toInt();

    if(pageNode.hasAttribute("tid"))
        m_currentPage->titleID = pageNode.attribute("tid").toInt();
    else
        getPageTitleID();

    if(m_showShorooh)
        getShorooh();

    QString pageText = getPageText(m_currentPage->pageID);
    m_textFormat->insertText(proccessPageText(pageText));

    m_textFormat->done();

    emit textChanged();
}

void RichSimpleBookReader::getShorooh()
{
    m_bookInfo->shorooh.clear();

    QDomElement linkElement = m_pagesDom.currentElement().firstChildElement("link");
    while(!linkElement.isNull()) {
        int bookID = linkElement.attribute("book").toInt();
        int page = linkElement.attribute("page").toInt();

        LibraryBook::Ptr book = LibraryManager::instance()->bookManager()->getLibraryBook(bookID);
        if(book)
            m_bookInfo->shorooh.append(BookShorooh(bookID, page, book->title));

        linkElement = linkElement.nextSiblingElement("link");
    }
}
