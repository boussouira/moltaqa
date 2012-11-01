#include "richbookreader.h"
#include "textformatter.h"
#include "librarybook.h"
#include "bookpage.h"
#include "utils.h"
#include "librarybookmanager.h"
#include "libraryinfo.h"
#include "modelenums.h"
#include "xmlutils.h"
#include "stringutils.h"

#include <qstandarditemmodel.h>

RichBookReader::RichBookReader(QObject *parent) : AbstractBookReader(parent)
{
    m_textFormat = 0;
    m_indexModel = 0;
    m_query = 0;
    m_highlightPageID = -1;
    m_stopModelLoad = false;

    m_bookmanager = m_libraryManager->bookManager();

    m_removeTashekil = Utils::Settings::get("Style/removeTashekil", false).toBool();
    m_fixShamelaShoorts = Utils::Settings::get("Style/fixShamelaShoorts", true).toBool();
    m_saveReadingHistory = true;

    connect(this, SIGNAL(textChanged()), SLOT(updateHistory()));
}

RichBookReader::~RichBookReader()
{
    ml_delete_check(m_textFormat);
    ml_delete_check(m_indexModel);
}

void RichBookReader::connected()
{
    m_textFormat->setData(m_bookInfo, m_currentPage);

    AbstractBookReader::connected();
}

QString RichBookReader::proccessPageText(QString text)
{
    if(m_removeTashekil)
        text = Utils::String::Arabic::removeTashekil(text);

    if(m_fixShamelaShoorts)
        text = Utils::String::Arabic::fixShamelaShoorts(text);

    if(m_query && m_highlightPageID == m_currentPage->pageID)
        text = Utils::CLucene::highlightText(text, m_query, false);

    return text;
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

        if(m_pageTitles.size())
            return m_pageTitles.first();
    }

    return pageID;
}

void RichBookReader::setRemoveTashkil(bool remove)
{
    m_removeTashekil = remove;
}

void RichBookReader::setSaveReadingHistory(bool save)
{
    m_saveReadingHistory = save;
}

QStandardItemModel *RichBookReader::indexModel()
{
    QuaZip zip;
    zip.setZipName(m_bookInfo->path);

    if(!zip.open(QuaZip::mdUnzip)) {
        qCritical() << "AbstractBookReader::openZip open book error"
                    << zip.getZipError() << "\n"
                    << "Book id:" << m_bookInfo->id << "\n"
                    << "Title:" << m_bookInfo->title << "\n"
                    << "Path:" << m_bookInfo->path;
        return 0;
    }

    QuaZipFile titleFile(&zip);

    if(zip.setCurrentFile("titles.xml")) {
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

    while(!element.isNull() && !m_stopModelLoad) {
        readItem(element, m_indexModel->invisibleRootItem());

        element = element.nextSiblingElement();
    }

    qSort(m_pageTitles);

    titleFile.close();
    zip.close();

    m_stopModelLoad = false;

    return m_indexModel;
}

void RichBookReader::stopModelLoad()
{
    m_stopModelLoad = true;
}

void RichBookReader::readItem(QDomElement &element, QStandardItem *parent)
{
    int pageID = element.attribute("pageID").toInt();

    QStandardItem *item = new QStandardItem(element.firstChildElement("text").text());
    item->setData(pageID, ItemRole::idRole);

    if(element.childNodes().count() > 1) {
        QDomElement child = element.firstChildElement("title");

        while(!child.isNull() && !m_stopModelLoad) {
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
    if(m_saveReadingHistory) {
        QtConcurrent::run(m_bookmanager,
                          &LibraryBookManager::addBookHistory,
                          m_bookInfo->id,
                          m_currentPage->pageID);
    }
}

TextFormatter *RichBookReader::textFormat()
{
    return m_textFormat;
}
