#include "texttafessirreader.h"
#include "librarybook.h"
#include "utils.h"
#include "bookutils.h"

TextTafessirReader::TextTafessirReader(QObject *parent) :
    TextBookReader(parent)
{
}

void TextTafessirReader::setCurrentPage(QDomElement pageNode)
{
    m_pagesDom.setCurrentElement(pageNode);
    m_currentPage->pageID = pageNode.attribute("id").toInt();
    m_currentPage->part = pageNode.attribute("part").toInt();
    m_currentPage->page = pageNode.attribute("page").toInt();

    m_currentPage->text = _u(m_pages.value(m_currentPage->pageID));
    m_pages.remove(m_currentPage->pageID);

//    if(!m_pages.contains(m_currentPage->pageID))
//        qWarning("Can't find page %d", m_currentPage->pageID);

    if(pageNode.hasAttribute("tid"))
        m_currentPage->titleID = pageNode.attribute("tid").toInt();
    else
        m_currentPage->titleID = Utils::Book::getPageTitleID(m_titles, m_currentPage->pageID);

//    if(m_titles.contains(m_currentPage->pageID))
//        m_currentPage->titleID = m_currentPage->pageID;
}
