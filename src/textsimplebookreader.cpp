#include "textsimplebookreader.h"
#include "librarybook.h"
#include "utils.h"

TextSimpleBookReader::TextSimpleBookReader(QObject *parent) :
    TextBookReader(parent)
{
}

TextSimpleBookReader::~TextSimpleBookReader()
{
    m_titles.clear();
}

void TextSimpleBookReader::setCurrentPage(QDomElement pageNode)
{
    m_currentElement = pageNode;
    m_currentPage->pageID = pageNode.attribute("id").toInt();
    m_currentPage->part = pageNode.attribute("part").toInt();
    m_currentPage->page = pageNode.attribute("page").toInt();

    m_currentPage->text = getFileContent(QString("pages/p%1.html").arg(m_currentPage->pageID));

    if(m_titles.contains(m_currentPage->pageID))
        m_currentPage->titleID = m_currentPage->pageID;
}
