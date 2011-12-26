#include "texttafessirreader.h"
#include "librarybook.h"
#include "utils.h"

TextTafessirReader::TextTafessirReader(QObject *parent) :
    TextBookReader(parent)
{
}

TextTafessirReader::~TextTafessirReader()
{
}

QString TextTafessirReader::text()
{
    return m_text;
}

void TextTafessirReader::firstPage()
{
    TextBookReader::firstPage();

    m_currentPage->titleID = m_titles.first();
}

void TextTafessirReader::setCurrentPage(QDomElement pageNode)
{
    m_currentElement = pageNode;
    m_currentPage->pageID = pageNode.attribute("id").toInt();
    m_currentPage->part = pageNode.attribute("part").toInt();
    m_currentPage->page = pageNode.attribute("page").toInt();

    m_text = getFileContent(QString("pages/p%1.html").arg(m_currentPage->pageID));

    if(m_titles.contains(m_currentPage->pageID))
        m_currentPage->titleID = m_currentPage->pageID;
}
