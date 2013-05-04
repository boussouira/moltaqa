#include "richbookreader.h"
#include "bookpage.h"
#include "bookutils.h"
#include "clutils.h"
#include "librarybook.h"
#include "librarybookmanager.h"
#include "libraryinfo.h"
#include "librarymanager.h"
#include "modelenums.h"
#include "stringutils.h"
#include "textformatter.h"
#include "utils.h"
#include "xmlutils.h"

#include <qstandarditemmodel.h>
#include <qxmlstream.h>

RichBookReader::RichBookReader(QObject *parent) : AbstractBookReader(parent)
{
    m_textFormat = 0;
    m_indexModel = 0;
    m_stopModelLoad = false;

    m_bookmanager = m_libraryManager->bookManager();

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
    m_textFormat->setData(m_book, m_currentPage);

    AbstractBookReader::connected();
}

QString RichBookReader::proccessPageText(QString text)
{
    if(m_removeTashekil)
        text = Utils::String::Arabic::removeTashekil(text);

    if(m_query && m_highlightPageID == m_currentPage->pageID)
        text = Utils::CLucene::highlightText(text, m_query, false);

    return text;
}

bool RichBookReader::scrollToHighlight()
{
    return (!m_book->isQuran() && m_query && m_highlightPageID == m_currentPage->pageID);
}

int RichBookReader::getPageTitleID(int pageID)
{
    return Utils::Book::getPageTitleID(m_pageTitles, pageID);
}

void RichBookReader::setSaveReadingHistory(bool save)
{
    m_saveReadingHistory = save;
}

QStandardItemModel *RichBookReader::indexModel()
{
    QuaZip zip;
    zip.setZipName(m_book->path);

    if(!zip.open(QuaZip::mdUnzip)) {
        qCritical() << "AbstractBookReader::openZip open book error"
                    << zip.getZipError() << "\n"
                    << "Book id:" << m_book->id << "\n"
                    << "Title:" << m_book->title << "\n"
                    << "Path:" << m_book->path;
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

    m_indexModel = new QStandardItemModel();
    m_pageTitles.clear();

    QStandardItem *rootItem = m_indexModel->invisibleRootItem();
    QXmlStreamReader reader(&titleFile);

    while(!reader.atEnd()) {
        reader.readNext();

        if(reader.isStartElement()) {
            if(reader.name() == "title") {
                int pageID = reader.attributes().value("pageID").toString().toInt();
                QString tid = reader.attributes().value("tagID").toString();

                if(reader.readNext() == QXmlStreamReader::Characters
                        && reader.readNext() == QXmlStreamReader::StartElement
                        && reader.name() == "text") {

                    if(reader.readNext() == QXmlStreamReader::Characters) {
                        QStandardItem *item = new QStandardItem(reader.text().toString());
                        item->setData(pageID, ItemRole::idRole);
                        item->setData(tid, ItemRole::titleIdRole);
                        rootItem->appendRow(item);

                        m_pageTitles.append(pageID);

                        rootItem = item;
                    } else {
                        if(reader.tokenType() != QXmlStreamReader::EndElement) { // Ignore empty titles
                            qWarning() << "RichBookReader::indexModel Unexpected token type"
                                       << reader.tokenString() << "- Book:" << m_book->id
                                       << m_book->title << m_book->fileName;
                        }

                        break;
                    }
                } else {
                    break;
                }
            }
        } else if(reader.isEndElement()) {
            if(reader.name() == "title") {
                if(rootItem
                        && rootItem != m_indexModel->invisibleRootItem()
                        && rootItem->parent()) {
                    rootItem = rootItem->parent();

                } else {
                    rootItem = m_indexModel->invisibleRootItem();
                }
            }
        }
    }

    if(reader.hasError()) {
        qDebug() << "RichBookReader::indexModel QXmlStreamReader error:" << reader.errorString()
                 << "- Book:" << m_book->id << m_book->title << m_book->fileName;
    }

    qSort(m_pageTitles);

    m_stopModelLoad = false;

    return m_indexModel;
}

void RichBookReader::stopModelLoad()
{
    m_stopModelLoad = true;
}

void RichBookReader::updateHistory()
{
    if(m_saveReadingHistory) {
        QtConcurrent::run(m_bookmanager,
                          &LibraryBookManager::addBookHistory,
                          m_book->id,
                          m_currentPage->pageID);
    }
}

TextFormatter *RichBookReader::textFormat()
{
    return m_textFormat;
}
