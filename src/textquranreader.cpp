#include "textquranreader.h"
#include "bookpage.h"

TextQuranReader::TextQuranReader(QObject *parent) :
    TextBookReader(parent)
{
}

void TextQuranReader::setCurrentPage(QDomElement pageNode)
{
    m_pagesDom.setCurrentElement(pageNode);
    m_currentPage->pageID = pageNode.attribute("id").toInt();
    m_currentPage->part = pageNode.attribute("part").toInt();
    m_currentPage->page = pageNode.attribute("page").toInt();
    m_currentPage->aya = pageNode.attribute("aya").toInt();
    m_currentPage->sora = pageNode.attribute("sora").toInt();

    m_currentPage->text = pageNode.text();

    m_currentPage->titleID = m_currentPage->sora;
}
