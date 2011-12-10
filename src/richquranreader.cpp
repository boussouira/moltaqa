#include "richquranreader.h"
#include "librarybook.h"
#include "bookindexmodel.h"
#include "bookexception.h"
#include "qurantextformat.h"
#include "libraryinfo.h"
#include "quranquery.h"
#include "utils.h"

#include <qsqldatabase.h>

RichQuranReader::RichQuranReader(QObject *parent) : RichBookReader(parent)
{
    m_formatter = new QuranTextFormat();
    m_textFormat = m_formatter;
}

RichQuranReader::~RichQuranReader()
{
}

void RichQuranReader::connected()
{
    RichBookReader::connected();
}

void RichQuranReader::goToPage(int pid)
{
    int page;
    if(pid == -1)       // First page number
        page = m_bookInfo->firstPage();
    else if(pid == -2)  // Last page number
        page = m_bookInfo->lastPage();
    else                // The given page number
        page = pid;

    goToPage(page, 1);

}
void RichQuranReader::goToPage(int page, int part)
{
    m_formatter->start();
    m_currentPage->page = page;
    m_currentPage->part = part;

    BookPage info = firstSoraAndAya(page);
    m_currentPage->sora = info.sora;
    m_currentPage->aya = info.aya;
    m_currentPage->ayatCount = info.ayatCount;

    QuranQuery quranQuery(m_bookDB, m_bookInfo);
    quranQuery.page(page);

    while(quranQuery.next()) {
        // at the first vers we insert the sora name and bassemala
        if(quranQuery.value(2).toInt() == 1) {
            m_formatter->insertSoraName(quranQuery.value(5).toString());

            // we escape putting bassemala before Fateha and Tawba
            if(quranQuery.value(4).toInt() != 1 && quranQuery.value(4).toInt() != 9)
                m_formatter->insertBassemala();
        }

        QString text;
        if(m_query && m_highlightPageID == quranQuery.value(0).toInt())
            text = Utils::highlightText(quranQuery.value(1).toString(),
                                        m_query, false);
        else
            text = quranQuery.value(1).toString();

        m_formatter->insertAyaText(text,
                                   quranQuery.value(2).toInt(),
                                   quranQuery.value(4).toInt());
    }

    m_currentPage->titleID = getPageTitleID(m_currentPage->pageID);

    m_formatter->done();

    emit textChanged();
}

void RichQuranReader::goToSora(int sora, int aya)
{
    int page = getPageNumber(sora, aya);

    if(page != m_currentPage->page)
        goToPage(page, 1);

    m_currentPage->sora = sora;
    m_currentPage->aya = aya;
    m_currentPage->ayatCount = getSoraAyatCount(sora);
}

bool RichQuranReader::needFastIndexLoad()
{
    return false;
}

BookIndexModel *RichQuranReader::indexModel()
{
    m_indexModel = new BookIndexModel();
    BookIndexNode *rootNode = new BookIndexNode();

    QuranQuery quranQuery(m_bookDB, m_bookInfo);

    quranQuery.index();
    while(quranQuery.next()) {
        BookIndexNode *firstChild = new BookIndexNode(quranQuery.value(1).toString(),
                                                      quranQuery.value(0).toInt());
        rootNode->appendChild(firstChild);
    }

    m_indexModel->setRootNode(rootNode);
    return m_indexModel;
}

int RichQuranReader::getPageTitleID(int pageID)
{
    Q_UNUSED(pageID);
    return m_currentPage->sora;
}

void RichQuranReader::getBookInfo()
{
    QuranQuery quranQuery(m_bookDB, m_bookInfo);
    quranQuery.prepare("SELECT  MIN(pageNumber), MAX(pageNumber), MIN(id), MAX(id) "
                          "FROM quranText");
    if(!quranQuery.exec()) {
        LOG_SQL_ERROR(quranQuery);
    }

    if(quranQuery.next()) {
        m_bookInfo->partsCount = 1;
        m_bookInfo->setFirstPage(quranQuery.value(0).toInt());
        m_bookInfo->setLastPage(quranQuery.value(1).toInt());

        m_bookInfo->firstID = quranQuery.value(2).toInt();
        m_bookInfo->lastID = quranQuery.value(3).toInt();
    }
}

int RichQuranReader::getPageNumber(int soraNumber, int ayaNumber)
{
    QuranQuery quranQuery(m_bookDB, m_bookInfo);
    quranQuery.pageNumber(ayaNumber, soraNumber);

    int page = 1;
    if(quranQuery.next()) {
        page = quranQuery.value(0).toInt();
    }

    return page;
}

void RichQuranReader::nextPage()
{
    if(hasNext()) {
        int page = m_currentPage->page+1;

        goToPage(page, 1);
    }
}

void RichQuranReader::prevPage()
{
    if(hasPrev()) {
        int page = m_currentPage->page-1;

        goToPage(page, 1);
    }
}

bool RichQuranReader::hasNext()
{
    return m_currentPage->page < m_bookInfo->lastPage();
}

bool RichQuranReader::hasPrev()
{
    return m_currentPage->page > m_bookInfo->firstPage();
}

void RichQuranReader::nextAya()
{
    int aya = m_currentPage->aya+1;
    int sora = m_currentPage->sora;
    if(aya > m_currentPage->ayatCount) {
        aya = 1;
        sora++;
    }
    if(sora > 114)
        sora = 1;
    int page = getPageNumber(sora, aya);

    if(page != m_currentPage->page)
        goToPage(page, 1);

    m_currentPage->sora = sora;
    m_currentPage->aya = aya;
    m_currentPage->ayatCount = getSoraAyatCount(sora);
}

void RichQuranReader::prevAya()
{
    int aya = m_currentPage->aya-1;
    int sora = m_currentPage->sora;
    if(aya < 1) {
        if(--sora < 1)
            sora = 114;

        aya = getSoraAyatCount(sora);
    }

    int page = getPageNumber(sora, aya);

    if(page != m_currentPage->page)
        goToPage(page, 1);

    m_currentPage->sora = sora;
    m_currentPage->aya = aya;
    m_currentPage->ayatCount = getSoraAyatCount(sora);
}

int RichQuranReader::getSoraAyatCount(int sora)
{
    QuranQuery quranQuery(m_bookDB, m_bookInfo);
    quranQuery.soraAyatCount(sora);

    return quranQuery.next() ? quranQuery.value(0).toInt() : 0;
}

BookPage RichQuranReader::firstSoraAndAya(int page)
{
    BookPage info;

    QuranQuery quranQuery(m_bookDB, m_bookInfo);
    quranQuery.firstSoraAndAya(page);
    if(quranQuery.next()) {
        // The first SORA number in page
        info.sora = quranQuery.value(0).toInt();
        // First aya number in page
        info.aya = quranQuery.value(1).toInt();
        info.ayatCount = getSoraAyatCount(quranQuery.value(0).toInt());
    }

    return info;
}
