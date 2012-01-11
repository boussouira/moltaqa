#include "richbookreader.h"
#include "textformatter.h"
#include "librarybook.h"
#include "bookpage.h"
#include "bookindexmodel.h"
#include "utils.h"
#include "mainwindow.h"
#include "libraryinfo.h"

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
    if(!m_pageTitles.isEmpty()) {

        if(m_pageTitles.contains(m_currentPage->pageID)) {
            m_currentPage->titleID = m_currentPage->pageID;
        } else {
            for(int i=0; i<m_pageTitles.size()-1; i++) {
                if(m_pageTitles.at(i) <= m_currentPage->pageID
                        && m_currentPage->pageID < m_pageTitles.at(i+1)) {
                    m_currentPage->titleID = m_pageTitles.at(i);
                    break;
                }
            }
        }
    }
}

BookIndexModel *RichBookReader::indexModel()
{
    m_pageTitles.clear();


    QuaZipFile titleFile(&m_zip);

    if(m_zip.setCurrentFile("titles.xml")) {
        if(!titleFile.open(QIODevice::ReadOnly)) {
            qWarning("testRead(): file.open(): %d", titleFile.getZipError());
            return 0;
        }
    }

    QDomDocument doc;
    QString errorStr;
    int errorLine=0;
    int errorColumn=0;

    if(!doc.setContent(&titleFile, 0, &errorStr, &errorLine, &errorColumn)) {
        qDebug("indexModel: Parse error at line %d, column %d: %s\nFile: %s",
               errorLine, errorColumn,
               qPrintable(errorStr),
               qPrintable(m_bookInfo->bookPath));

        titleFile.close();
        return 0;
    }

    BookIndexNode *rootNode = new BookIndexNode();
    QDomElement root = doc.documentElement();
    QDomElement element = root.firstChildElement();

    while(!element.isNull()) {
        readItem(element, rootNode);

        element = element.nextSiblingElement();
    }

    m_indexModel = new BookIndexModel();
    m_indexModel->setRootNode(rootNode);

    qSort(m_pageTitles);

    return m_indexModel;
}

void RichBookReader::readItem(QDomElement &element, BookIndexNode *parent)
{
    BookIndexNode *item = new BookIndexNode(element.attribute("text"),
                                            element.attribute("pageID").toInt());

    if(element.hasChildNodes()) {
        QDomElement child = element.firstChildElement();

        while(!child.isNull()) {
            readItem(child, item);

            child = child.nextSiblingElement();
        }
    }

    parent->appendChild(item);
    m_pageTitles.append(item->id);
}

TextFormatter *RichBookReader::textFormat()
{
    return m_textFormat;
}
