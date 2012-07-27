#include "textsimplebookreader.h"
#include "librarybook.h"
#include "utils.h"

TextSimpleBookReader::TextSimpleBookReader(QObject *parent) :
    TextBookReader(parent)
{
    m_loadTitlesText = true;
    m_lastTitleID = -1;
}

void TextSimpleBookReader::setCurrentPage(QDomElement pageNode)
{
    m_pagesDom.setCurrentElement(pageNode);
    m_currentPage->pageID = pageNode.attribute("id").toInt();
    m_currentPage->part = pageNode.attribute("part").toInt();
    m_currentPage->page = pageNode.attribute("page").toInt();

    m_currentPage->text = _u(m_pages.value(m_currentPage->pageID));

//    if(!m_pages.contains(m_currentPage->pageID))
//        qWarning("Can't find page %d", m_currentPage->pageID);

    if(pageNode.hasAttribute("tid"))
        m_currentPage->titleID = pageNode.attribute("tid").toInt();
    else
        m_currentPage->titleID = getPageTitleID(m_currentPage->pageID);

    if(m_lastTitleID != m_currentPage->titleID) {
        m_lastTitleID = m_currentPage->titleID;
        m_currentPage->title = m_titlesText[m_lastTitleID];
    } else {
        m_currentPage->title.clear();
    }
}
