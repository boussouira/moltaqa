#include "richtafessirreader.h"
#include "tafessirtextformat.h"
#include "librarybook.h"
#include "bookindexmodel.h"
#include "bookindexnode.h"
#include "libraryinfo.h"
#include "bookindexmodel.h"
#include "bookindexnode.h"
#include "bookexception.h"
#include "utils.h"
#include "mainwindow.h"
#include "bookreaderhelper.h"

#include <qsqlquery.h>
#include <qstringlist.h>
#include <qsqlerror.h>
#include <qdebug.h>
#include <qdatetime.h>

RichTafessirReader::RichTafessirReader(QObject *parent, bool showQuran) : RichBookReader(parent)
{
    m_formatter = new TafessirTextFormat();
    m_textFormat = m_formatter;
    m_quranInfo = 0;
    m_showQuran = showQuran;
}

RichTafessirReader::~RichTafessirReader()
{
    if(m_quranDB.isOpen()) {
        QString conn = m_quranDB.connectionName();
        m_quranDB.close();
        m_quranDB = QSqlDatabase();
        QSqlDatabase::removeDatabase(conn);
    }
}

void RichTafessirReader::connected()
{
    m_quranInfo = m_libraryManager->getQuranBook();
    if(m_quranInfo && m_showQuran)
        openQuranBook();

    RichBookReader::connected();
}

void RichTafessirReader::setCurrentPage(QDomElement pageNode)
{
    m_formatter->start();

    m_currentElement = pageNode;

    m_currentPage->pageID = m_currentElement.attribute("id").toInt();
    m_currentPage->page = m_currentElement.attribute("page").toInt();
    m_currentPage->part = m_currentElement.attribute("part").toInt();
    m_currentPage->sora = m_currentElement.attribute("sora").toInt();
    m_currentPage->aya = m_currentElement.attribute("aya").toInt();

    getPageTitleID();

    // TODO: don't show quran text when browsing tafessir book directly?
//    if(m_quranInfo && m_showQuran) {
//        readQuranText(m_currentPage->sora,
//                      m_currentPage->aya,
//                      tafessirQuery.getAyatCount(m_currentPage->sora, m_currentPage->aya));
//    }

    QString pageText = getFileContent(QString("pages/p%1.html").arg(m_currentPage->pageID));
    if(m_query && m_highlightPageID == m_currentPage->pageID)
        m_textFormat->insertText(Utils::highlightText(pageText, m_query, false));
    else
        m_textFormat->insertText(pageText);

    m_formatter->done();

    emit textChanged();
}

QDomElement RichTafessirReader::getQuranPageId(int sora, int aya)
{
    QString soraNum = QString::number(sora);

    QDomElement e = m_rootElement.lastChildElement();
    while(!e.isNull()) {

        if(soraNum == e.attribute("sora")) {
            if(aya >= e.attribute("aya").toInt()) {
                break;
            }
        }

        e = e.previousSiblingElement();
    }

    if(!e.isNull()) {
        QString current = e.attribute("aya");

        while(!e.isNull()) {
            if(e.attribute("aya") != current)
                return e.nextSiblingElement();

            e = e.previousSiblingElement();
        }
    }

    return QDomElement();
}

void RichTafessirReader::openQuranBook()
{
    QString connName = QString("quran_tafessir_%1").arg(m_bookInfo->bookID);

    while(QSqlDatabase::contains(connName)) {
        connName.append("_");
    }

    m_quranDB = QSqlDatabase::addDatabase("QSQLITE", connName);
    m_quranDB.setDatabaseName(m_quranInfo->bookPath);

    if (!m_quranDB.open())
        throw BookException(tr("لم يمكن فتح قاعدة البيانات"), m_quranInfo->bookPath);
}

void RichTafessirReader::readQuranText(int sora, int aya, int count)
{
    if(count>0) {
        QSqlQuery quranQuery(m_quranDB);
        QuranSora *soraInfo = MW->readerHelper()->getQuranSora(sora);

        m_formatter->beginQuran(soraInfo->name, aya, aya+count-1);

        quranQuery.exec(QString("SELECT quranText.ayaText, quranText.ayaNumber, "
                                   "quranText.soraNumber "
                                   "FROM quranText "
                                   "WHERE quranText.ayaNumber >= %1 AND quranText.soraNumber = %2 "
                                   "ORDER BY quranText.id LIMIT %3").arg(aya).arg(sora).arg(count));
        while(quranQuery.next()) {
            m_formatter->insertAyaText(quranQuery.value(0).toString(),
                                       quranQuery.value(1).toInt(),
                                       quranQuery.value(2).toInt());
        }

        m_formatter->endQuran();
    }
}
