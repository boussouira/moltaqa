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
    if(m_quranZip.isOpen())
        m_quranZip.close();
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
    if(m_quranInfo && m_showQuran) {
        int nextAya = nextAyaNumber(pageNode);
        readQuranText(m_currentPage->sora,
                      m_currentPage->aya,
                      nextAya-m_currentPage->aya);
    }

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
    Q_CHECK_PTR(m_quranInfo);

    if(!QFile::exists(m_quranInfo->bookPath)) {
        throw BookException(tr("لم يتم العثور على ملف الكتاب"), bookInfo()->bookPath);
    }

    m_quranZipFile.setFileName(m_quranInfo->bookPath);
    m_quranZip.setIoDevice(&m_quranZipFile);

    if(!m_quranZip.open(QuaZip::mdUnzip)) {
        throw BookException(tr("لا يمكن فتح ملف الكتاب"), m_quranInfo->bookPath, m_quranZip.getZipError());
    }

    QString errorStr;
    int errorLine=0;
    int errorColumn=0;

    m_quranPages.setZip(&m_quranZip);

    if(m_quranZip.setCurrentFile("pages.xml")) {
        if(!m_quranPages.open(QIODevice::ReadOnly)) {
            qWarning("getBookInfo: open error %d", m_quranPages.getZipError());
            return;
        }
    }

    if(!m_qurankDoc.setContent(&m_quranPages, 0, &errorStr, &errorLine, &errorColumn)) {
        qDebug("openQuranBook: Parse error at line %d, column %d: %s\nFile: %s",
               errorLine, errorColumn,
               qPrintable(errorStr),
               qPrintable(m_quranInfo->bookPath));

        return;
    }

    m_quranRootElement = m_qurankDoc.documentElement();
}

void RichTafessirReader::readQuranText(int sora, int aya, int count)
{
    count = qMax(count, 1);

    QString soraStr = QString::number(sora);
    QString ayaStr = QString::number(aya);

    QDomElement e = m_quranRootElement.firstChildElement();
    while(!e.isNull()) {
        // find the aya
        if(e.attribute("aya")==ayaStr && e.attribute("sora") == soraStr) {
            QuranSora *soraInfo = MW->readerHelper()->getQuranSora(sora);

            m_formatter->beginQuran(soraInfo->name, aya, aya+count-1);

            for(int i=0; i<count; i++) {
                m_formatter->insertAyaText(e.text(),
                                           e.attribute("aya").toInt(),
                                           e.attribute("sora").toInt());

                e = e.nextSiblingElement();
            }

            m_formatter->endQuran();
            break;
        }

        e = e.nextSiblingElement();
    }
}

int RichTafessirReader::nextAyaNumber(QDomElement e)
{
    QString aya = e.attribute("aya");
    QString sora = e.attribute("sora");

    while(!e.isNull()) {
        if(e.attribute("sora")==sora) {
            if(e.attribute("aya")!=aya) {
                return e.attribute("aya").toInt();
            }
        } else {
            break;
        }

        e = e.nextSiblingElement();
    }

    return 0;
}
