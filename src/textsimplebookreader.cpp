#include "textsimplebookreader.h"
#include "librarybook.h"

TextSimpleBookReader::TextSimpleBookReader(QObject *parent) :
    TextBookReader(parent)
{
}

void TextSimpleBookReader::setCurrentPage(QDomElement pageNode)
{
    m_pagesDom.setCurrentElement(pageNode);
    m_currentPage->pageID = pageNode.attribute("id").toInt();
    m_currentPage->part = pageNode.attribute("part").toInt();
    m_currentPage->page = pageNode.attribute("page").toInt();

    m_currentPage->text = QString::fromUtf8(m_pages.value(m_currentPage->pageID));

//    if(!m_pages.contains(m_currentPage->pageID))
//        qWarning("Can't find page %d", m_currentPage->pageID);

    m_currentPage->titleID = getPageTitleID(m_currentPage->pageID);

//    if(m_titles.contains(m_currentPage->pageID))
//        m_currentPage->titleID = m_currentPage->pageID;
}
