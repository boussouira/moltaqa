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
    m_quranQuery = 0;
}

RichQuranReader::~RichQuranReader()
{
    delete m_formatter;

    if(m_quranQuery)
        delete m_quranQuery;
}

void RichQuranReader::connected()
{
    m_quranQuery = new QuranQuery(m_bookDB, m_bookInfo);
    AbstractBookReader::connected();
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
    m_bookInfo->currentPage.page = page;
    m_bookInfo->currentPage.part = part;

    BookPage info = firstSoraAndAya(page);
    m_bookInfo->currentPage.sora = info.sora;
    m_bookInfo->currentPage.aya = info.aya;
    m_bookInfo->currentPage.ayatCount = info.ayatCount;

    m_quranQuery->page(page);

    while(m_quranQuery->next()) {
        // at the first vers we insert the sora name and bassemala
        if(m_quranQuery->value(2).toInt() == 1) {
            m_formatter->insertSoraName(m_quranQuery->value(5).toString());

            // we escape putting bassemala before Fateha and Tawba
            if(m_quranQuery->value(4).toInt() != 1 && m_quranQuery->value(4).toInt() != 9)
                m_formatter->insertBassemala();
        }
        m_formatter->insertAyaText(m_quranQuery->value(1).toString(),
                                     m_quranQuery->value(2).toInt(),
                                     m_quranQuery->value(4).toInt());
    }

    m_formatter->done();

    emit textChanged();
}

void RichQuranReader::goToSora(int sora, int aya)
{
    int page = getPageNumber(sora, aya);
    m_bookInfo->currentPage.sora = sora;
    m_bookInfo->currentPage.aya = aya;
    m_bookInfo->currentPage.ayatCount = getSoraAyatCount(sora);

    if(page != m_bookInfo->currentPage.page)
        goToPage(page, 1);
}

bool RichQuranReader::needFastIndexLoad()
{
    return false;
}

QAbstractItemModel *RichQuranReader::indexModel()
{
    BookIndexNode *rootNode = new BookIndexNode();

    m_quranQuery->index();
    while(m_quranQuery->next()) {
        BookIndexNode *firstChild = new BookIndexNode(m_quranQuery->value(1).toString(),
                                                      m_quranQuery->value(0).toInt());
        rootNode->appendChild(firstChild);
    }

    m_indexModel->setRootNode(rootNode);
    return m_indexModel;
}

void RichQuranReader::getBookInfo()
{
    m_quranQuery->prepare("SELECT  MIN(pageNumber), MAX(pageNumber), MIN(id), MAX(id) "
                          "FROM quranText");
    if(!m_quranQuery->exec()) {
        LOG_SQL_P_ERROR(m_quranQuery);
    }

    if(m_quranQuery->next()) {
        m_bookInfo->partsCount = 1;
        m_bookInfo->setFirstPage(m_quranQuery->value(0).toInt());
        m_bookInfo->setLastPage(m_quranQuery->value(1).toInt());

        m_bookInfo->firstID = m_quranQuery->value(2).toInt();
        m_bookInfo->lastID = m_quranQuery->value(3).toInt();
    }
}

int RichQuranReader::getPageNumber(int soraNumber, int ayaNumber)
{
    int page = 1;
    m_quranQuery->pageNumber(ayaNumber, soraNumber);

    if(m_quranQuery->next()) {
        page = m_quranQuery->value(0).toInt();
        m_bookInfo->currentPage.aya = 1;
        m_bookInfo->currentPage.sora = soraNumber;
    }
    return page;
}

void RichQuranReader::nextPage()
{
    if(hasNext()) {
        int page = m_bookInfo->currentPage.page+1;

        goToPage(page, 1);
    }
}

void RichQuranReader::prevPage()
{
    if(hasPrev()) {
        int page = m_bookInfo->currentPage.page-1;

        goToPage(page, 1);
    }
}

bool RichQuranReader::hasNext()
{
    return m_bookInfo->currentPage.page < m_bookInfo->lastPage();
}

bool RichQuranReader::hasPrev()
{
    return m_bookInfo->currentPage.page > m_bookInfo->firstPage();
}

void RichQuranReader::nextAya()
{
    int aya = m_bookInfo->currentPage.aya+1;
    int sora = m_bookInfo->currentPage.sora;
    if(aya > m_bookInfo->currentPage.ayatCount) {
        aya = 1;
        sora++;
    }
    if(sora > 114)
        sora = 1;
    int page = getPageNumber(sora, aya);
    m_bookInfo->currentPage.sora = sora;
    m_bookInfo->currentPage.aya = aya;
    m_bookInfo->currentPage.ayatCount = getSoraAyatCount(sora);

    if(page != m_bookInfo->currentPage.page)
        goToPage(page, 1);

}

void RichQuranReader::prevAya()
{
    int aya = m_bookInfo->currentPage.aya-1;
    int sora = m_bookInfo->currentPage.sora;
    if(aya < 1) {
        if(--sora < 1)
            sora = 114;

        aya = getSoraAyatCount(sora);
    }

    int page = getPageNumber(sora, aya);

    m_bookInfo->currentPage.sora = sora;
    m_bookInfo->currentPage.aya = aya;
    m_bookInfo->currentPage.ayatCount = getSoraAyatCount(sora);

    if(page != m_bookInfo->currentPage.page)
        goToPage(page, 1);
}

int RichQuranReader::getSoraAyatCount(int sora)
{
    m_quranQuery->soraAyatCount(sora);

    return m_quranQuery->next() ? m_quranQuery->value(0).toInt() : 0;
}

BookPage RichQuranReader::firstSoraAndAya(int page)
{
    BookPage info;

    m_quranQuery->firstSoraAndAya(page);
    if(m_quranQuery->next()) {
        // The first SORA number in page
        info.sora = m_quranQuery->value(0).toInt();
        // First aya number in page
        info.aya = m_quranQuery->value(1).toInt();
        info.ayatCount = getSoraAyatCount(m_quranQuery->value(0).toInt());
    }

    return info;
}
