#include "richsimplebookreader.h"
#include "abstractbookreader.h"
#include "librarybook.h"
#include "bookexception.h"
#include "simpletextformat.h"
#include "libraryinfo.h"
#include "librarybookmanager.h"

#include <qstringlist.h>
#include <qdebug.h>
#include <qdatetime.h>

RichSimpleBookReader::RichSimpleBookReader(QObject *parent) : RichBookReader(parent)
{
    m_textFormat = new SimpleTextFormat();
}

RichSimpleBookReader::~RichSimpleBookReader()
{
}

void RichSimpleBookReader::connected()
{
    RichBookReader::connected();
}

void RichSimpleBookReader::setCurrentPage(QDomElement pageNode)
{
    m_textFormat->start();

    m_pagesDom.setCurrentElement(pageNode);

    m_currentPage->pageID = m_pagesDom.currentElement().attribute("id").toInt();
    m_currentPage->page = m_pagesDom.currentElement().attribute("page").toInt();
    m_currentPage->part = m_pagesDom.currentElement().attribute("part").toInt();
    m_currentPage->haddit = m_pagesDom.currentElement().attribute("haddit").toInt();

    if(m_pagesDom.currentElement().hasAttribute("tid"))
        m_currentPage->titleID = m_pagesDom.currentElement().attribute("tid").toInt();
    else
        getPageTitleID();

    getShorooh();

    QString pageText = getPageText(m_currentPage->pageID);
    if(m_query && m_highlightPageID == m_currentPage->pageID)
        m_textFormat->insertText(Utils::highlightText(pageText, m_query, false));
    else
        m_textFormat->insertText(pageText);

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

        LibraryBookPtr book = LibraryBookManager::instance()->getLibraryBook(bookID);
        if(book)
            m_bookInfo->shorooh.append(BookShorooh(bookID, page, book->bookDisplayName));

        linkElement = linkElement.nextSiblingElement("link");
    }
}
