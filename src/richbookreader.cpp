#include "richbookreader.h"
#include "textformatter.h"
#include "librarybook.h"
#include "bookpage.h"
#include "utils.h"
#include "librarybookmanager.h"
#include "libraryinfo.h"
#include "modelenums.h"
#include "xmlutils.h"

#include <qstandarditemmodel.h>

RichBookReader::RichBookReader(QObject *parent) : AbstractBookReader(parent)
{
    m_textFormat = 0;
    m_query = 0;
    m_highlightPageID = -1;

    m_bookmanager = m_libraryManager->bookManager();

    QSettings settings;
    m_removeTashekil = settings.value("Style/removeTashekil", false).toBool();

    connect(this, SIGNAL(textChanged()), SLOT(updateHistory()));
}

RichBookReader::~RichBookReader()
{
    ml_delete_check(m_textFormat);
}

void RichBookReader::connected()
{
    m_textFormat->setData(m_bookInfo, m_currentPage);

    AbstractBookReader::connected();
}

void RichBookReader::highlightPage(int pageID, CLuceneQuery *query)
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
        int title = 0;
        for(int i=0; i<m_pageTitles.size(); i++) {
            title = m_pageTitles.at(i);
            if(i == m_pageTitles.size()-1)
                return m_pageTitles.last();
            else if(title <= pageID && pageID < m_pageTitles.at(i+1))
                return title;
            if(title > pageID)
                break;
        }

        return m_pageTitles.first();
    }

    return pageID;
}

QStandardItemModel *RichBookReader::indexModel()
{
    ZipOpener opener(this);
    QuaZipFile titleFile(&m_zip);

    if(m_zip.setCurrentFile("titles.xml")) {
        if(!titleFile.open(QIODevice::ReadOnly)) {
            qWarning("RichBookReader::indexModel open title file error %d",
                     titleFile.getZipError());
            return 0;
        }
    }

    QDomDocument doc = Utils::Xml::getDomDocument(&titleFile);
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

    QStandardItem *item = new QStandardItem(element.firstChildElement("text").text());
    item->setData(pageID, ItemRole::idRole);

    if(element.childNodes().count() > 1) {
        QDomElement child = element.firstChildElement("title");

        while(!child.isNull()) {
            readItem(child, item);

            child = child.nextSiblingElement("title");
        }
    }

    if(parent)
        parent->appendRow(item);

    m_pageTitles.append(pageID);
}

void RichBookReader::updateHistory()
{
    QtConcurrent::run(m_bookmanager,
                      &LibraryBookManager::addBookHistory,
                      m_bookInfo->id,
                      m_currentPage->pageID);
}

TextFormatter *RichBookReader::textFormat()
{
    return m_textFormat;
}
