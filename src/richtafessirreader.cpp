#include "richtafessirreader.h"
#include "bookexception.h"
#include "bookreaderhelper.h"
#include "bookutils.h"
#include "librarybook.h"
#include "librarybookmanager.h"
#include "libraryinfo.h"
#include "librarymanager.h"
#include "mainwindow.h"
#include "stringutils.h"
#include "tafessirtextformat.h"
#include "utils.h"
#include "xmlutils.h"

#include <qdatetime.h>
#include <qstringlist.h>

RichTafessirReader::RichTafessirReader(QObject *parent) : RichBookReader(parent)
{
    m_formatter = new TafessirTextFormat();
    m_textFormat = m_formatter;
    m_showQuran = true;
}

RichTafessirReader::~RichTafessirReader()
{
}

void RichTafessirReader::setShowQuranText(bool show)
{
    m_showQuran = show;
}

void RichTafessirReader::connected()
{
    m_quranInfo = LibraryManager::instance()->bookManager()->getQuranBook();
    if(m_quranInfo && m_showQuran)
        openQuranBook();

    RichBookReader::connected();
}

void RichTafessirReader::setCurrentPage(QDomElement pageNode)
{
    m_formatter->start();

    m_pagesDom.setCurrentElement(pageNode);

    m_currentPage->pageID = pageNode.attribute("id").toInt();
    m_currentPage->page = pageNode.attribute("page").toInt();
    m_currentPage->part = pageNode.attribute("part").toInt();
    m_currentPage->haddit = pageNode.attribute("haddit").toInt();
    m_currentPage->sora = pageNode.attribute("sora").toInt();
    m_currentPage->aya = pageNode.attribute("aya").toInt();

    if(pageNode.hasAttribute("tid"))
        m_currentPage->titleID = pageNode.attribute("tid").toInt();
    else
        m_currentPage->titleID = Utils::Book::getPageTitleID(m_pageTitles, m_currentPage->pageID);

    // TODO: don't show quran text when browsing tafessir book directly?
    if(m_quranInfo && m_showQuran) {
        int nextAya = nextAyaNumber(pageNode);
        readQuranText(m_currentPage->sora,
                      m_currentPage->aya,
                      nextAya-m_currentPage->aya);
    }

    QString pageText = getPageText(m_currentPage->pageID);
    m_textFormat->insertText(proccessPageText(pageText));

    m_formatter->done();

    emit textChanged();
}

QDomElement RichTafessirReader::getQuranPageId(int sora, int aya)
{
    QString soraNum = QString::number(sora);

    QDomElement e = m_pagesDom.rootElement().lastChildElement();
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
    ml_return_on_fail2(m_quranInfo, "RichTafessirReader::openQuranBook Quran book is null");

    if(!QFile::exists(m_quranInfo->path)) {
        throw BookException(tr("لم يتم العثور على ملف الكتاب"), book()->path);
    }

    QuaZip quranZip;
    quranZip.setZipName(m_quranInfo->path);

    if(!quranZip.open(QuaZip::mdUnzip)) {
        throw BookException(tr("لا يمكن فتح ملف الكتاب"), m_quranInfo->path, quranZip.getZipError());
    }

    QuaZipFile quranPages;
    quranPages.setZip(&quranZip);

    ml_return_on_fail2(quranZip.setCurrentFile("pages.xml"), "openQuranBook: setCurrentFile error" << quranZip.getZipError());
    ml_return_on_fail2(quranPages.open(QIODevice::ReadOnly), "openQuranBook: open error" << quranPages.getZipError());

    m_quranDom.load(&quranPages);
}

void RichTafessirReader::readQuranText(int sora, int aya, int count)
{
    count = qMax(count, 1);

    QString soraStr = QString::number(sora);
    QString ayaStr = QString::number(aya);

    QDomElement e = m_quranDom.rootElement().firstChildElement();
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
