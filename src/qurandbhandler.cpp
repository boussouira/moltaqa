#include "qurandbhandler.h"
#include "bookinfo.h"
#include "bookindexmodel.h"
#include "bookexception.h"
#include "qurantextformat.h"
#include "libraryinfo.h"
#include "quranquery.h"
#include "utils.h"

#include <qsqldatabase.h>

QuranDBHandler::QuranDBHandler()
{
    m_formatter = new QuranTextFormat();
    m_textFormat = m_formatter;
    m_fastIndex = false;
    m_quranQuery = 0;
}

QuranDBHandler::~QuranDBHandler()
{
    delete m_formatter;

    if(m_quranQuery)
        delete m_quranQuery;
}

void QuranDBHandler::connected()
{
    m_quranQuery = new QuranQuery(m_bookDB, m_bookInfo);
    AbstractDBHandler::connected();
}

void QuranDBHandler::openID(int pid)
{
    int page;
    if(pid == -1)       // First page number
        page = m_bookInfo->firstPage();
    else if(pid == -2)  // Last page number
        page = m_bookInfo->lastPage();
    else                // The given page number
        page = pid;

    openPage(page);

}
void QuranDBHandler::openPage(int page, int part)
{
    m_formatter->start();
    m_bookInfo->currentPageNumber = page;
    m_bookInfo->currentPartNumber = part;


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
}

void QuranDBHandler::openSora(int num)
{
    openPage(getPageNumber(num));
}

void QuranDBHandler::openIndexID(int pid)
{
    int sora = qMax(pid, 1);
    m_bookInfo->currentSoraNumber = sora;
    m_bookInfo->currentAyaNumber = 1;
    m_bookInfo->currentSoraAyatCount = getSoraAyatCount(sora);

    openPage(getPageNumber(sora));
}

QAbstractItemModel *QuranDBHandler::indexModel()
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

void QuranDBHandler::getBookInfo()
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

int QuranDBHandler::getPageNumber(int soraNumber, int ayaNumber)
{
    int page = 1;
    m_quranQuery->pageNumber(ayaNumber, soraNumber);

    if(m_quranQuery->next()) {
        page = m_quranQuery->value(0).toInt();
        m_bookInfo->currentAyaNumber = 1;
        m_bookInfo->currentSoraNumber = soraNumber;
    }
    return page;
}

void QuranDBHandler::nextPage()
{
    if(hasNext()) {
        int page = m_bookInfo->currentPageNumber+1;
        firstSoraAndAya(page);

        openPage(page);
    }
}

void QuranDBHandler::prevPage()
{
    if(hasPrev()) {
        int page = m_bookInfo->currentPageNumber-1;
        firstSoraAndAya(page);

        openPage(page);
    }
}

bool QuranDBHandler::hasNext()
{
    return m_bookInfo->currentPageNumber < m_bookInfo->lastPage();
}

bool QuranDBHandler::hasPrev()
{
    return m_bookInfo->currentPageNumber > m_bookInfo->firstPage();
}

void QuranDBHandler::nextUnit()
{
    int aya = m_bookInfo->currentAyaNumber+1;
    int sora = m_bookInfo->currentSoraNumber;
    if(aya > m_bookInfo->currentSoraAyatCount) {
        aya = 1;
        sora++;
    }
    if(sora > 114)
        sora = 1;
    int page = getPageNumber(sora, aya);
    m_bookInfo->currentSoraNumber = sora;
    m_bookInfo->currentAyaNumber = aya;
    m_bookInfo->currentSoraAyatCount = getSoraAyatCount(sora);

    if(page != m_bookInfo->currentPageNumber)
        openPage(page);

}

void QuranDBHandler::prevUnit()
{
    int aya = m_bookInfo->currentAyaNumber-1;
    int sora = m_bookInfo->currentSoraNumber;
    if(aya < 1) {
        aya = 1;
        sora--;
    }
    if(sora < 1)
        sora = 114;
    int page = getPageNumber(sora, aya);
    m_bookInfo->currentSoraNumber = sora;
    m_bookInfo->currentAyaNumber = aya;
    m_bookInfo->currentSoraAyatCount = getSoraAyatCount(sora);

    if(page != m_bookInfo->currentPageNumber)
        openPage(page);
}

int QuranDBHandler::getSoraAyatCount(int sora)
{
    m_quranQuery->soraAyatCount(sora);

    return m_quranQuery->next() ? m_quranQuery->value(0).toInt() : 0;
}

void QuranDBHandler::firstSoraAndAya(int page)
{
    m_quranQuery->firstSoraAndAya(page);
    if(m_quranQuery->next()) {
        // The first SORA number in page
        m_bookInfo->currentSoraNumber = m_quranQuery->value(0).toInt();
        // First aya number in page
        m_bookInfo->currentAyaNumber = m_quranQuery->value(1).toInt();
        m_bookInfo->currentSoraAyatCount = getSoraAyatCount(m_quranQuery->value(0).toInt());
    }
}

QAbstractItemModel * QuranDBHandler::topIndexModel()
{
    return 0;
}

void QuranDBHandler::goToPage(int page, int part)
{
    firstSoraAndAya(page);
    openPage(page, part);
}

void QuranDBHandler::goToSora(int sora, int aya)
{
    int page = getPageNumber(sora, aya);
    m_bookInfo->currentSoraNumber = sora;
    m_bookInfo->currentAyaNumber = aya;
    m_bookInfo->currentSoraAyatCount = getSoraAyatCount(sora);

    if(page != m_bookInfo->currentPageNumber)
        openPage(page);
}

void QuranDBHandler::goToHaddit(int /*hadditNum*/)
{

}
