#include "richsimplebookreader.h"
#include "abstractbookreader.h"
#include "librarybook.h"
#include "bookexception.h"
#include "simpletextformat.h"
#include "libraryinfo.h"

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

    m_currentElement = pageNode;

    m_currentPage->pageID = m_currentElement.attribute("id").toInt();
    m_currentPage->page = m_currentElement.attribute("page").toInt();
    m_currentPage->part = m_currentElement.attribute("part").toInt();

    getPageTitleID();
    m_libraryManager->getShoroohPages(m_bookInfo, m_currentPage);

    QString pageText = getFileContent(QString("pages/p%1.html").arg(m_currentPage->pageID));
    if(m_query && m_highlightPageID == m_currentPage->pageID)
        m_textFormat->insertText(Utils::highlightText(pageText, m_query, false));
    else
        m_textFormat->insertText(pageText);

    m_textFormat->done();

    emit textChanged();
}
