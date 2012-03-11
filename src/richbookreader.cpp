#include "richbookreader.h"
#include "textformatter.h"
#include "librarybook.h"
#include "bookpage.h"
#include "utils.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "modelenums.h"
#include "xmlutils.h"

#include <qstandarditemmodel.h>

RichBookReader::RichBookReader(QObject *parent) : AbstractBookReader(parent)
{
    m_textFormat = 0;
    m_query = 0;
    m_highlightPageID = -1;
}

RichBookReader::~RichBookReader()
{
    if(m_textFormat)
        delete m_textFormat;
}

void RichBookReader::connected()
{
    m_textFormat->setData(m_bookInfo, m_currentPage);

    AbstractBookReader::connected();
}

void RichBookReader::highlightPage(int pageID, lucene::search::Query *query)
{
    m_query = query;
    m_highlightPageID = pageID;
}

bool RichBookReader::scrollToHighlight()
{
    return (!m_bookInfo->isQuran() && m_query && m_highlightPageID == m_currentPage->pageID);
}

void RichBookReader::getPageTitleID()
{
    m_currentPage->titleID = getPageTitleID(m_currentPage->pageID);
}

int RichBookReader::getPageTitleID(int pageID)
{
    if(!m_pageTitles.contains(pageID)) {
        for(int i=0; i<m_pageTitles.size()-1; i++) {
            if(m_pageTitles.at(i) <= pageID && pageID < m_pageTitles.at(i+1))
                return m_pageTitles.at(i);
        }
    }

    return pageID;
}

QStandardItemModel *RichBookReader::indexModel()
{
    ZipOpener opener(this);
    QuaZipFile titleFile(&m_zip);

    if(m_zip.setCurrentFile("titles.xml")) {
        if(!titleFile.open(QIODevice::ReadOnly)) {
            qWarning("testRead(): file.open(): %d", titleFile.getZipError());
            return 0;
        }
    }

    QDomDocument doc = Utils::getDomDocument(&titleFile);
    if(doc.isNull())
        return 0;

    QDomElement root = doc.documentElement();
    QDomElement element = root.firstChildElement();

    m_indexModel = new QStandardItemModel();
    m_pageTitles.clear();

    while(!element.isNull()) {
        readItem(element, m_indexModel->invisibleRootItem());

        element = element.nextSiblingElement();
    }

    qSort(m_pageTitles);

    return m_indexModel;
}

void RichBookReader::readItem(QDomElement &element, QStandardItem *parent)
{
    int pageID = element.attribute("pageID").toInt();

    QStandardItem *item = new QStandardItem(element.attribute("text"));
    item->setData(pageID, ItemRole::idRole);

    if(element.hasChildNodes()) {
        QDomElement child = element.firstChildElement();

        while(!child.isNull()) {
            readItem(child, item);

            child = child.nextSiblingElement();
        }
    }

    if(parent)
        parent->appendRow(item);

    m_pageTitles.append(pageID);
}

TextFormatter *RichBookReader::textFormat()
{
    return m_textFormat;
}
