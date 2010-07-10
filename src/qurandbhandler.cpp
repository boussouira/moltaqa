#include "qurandbhandler.h"
#include <QSqlError>
#include <qdebug.h>

QuranDBHandler::QuranDBHandler()
{
    m_quranFormat = new QuranTextFormat();
}

QuranDBHandler::~QuranDBHandler()
{
    delete m_quranFormat;
}

QString QuranDBHandler::page(int pid)
{
    int page;
    if(pid == -1)       // First page number
        page = m_bookInfo->firstPage();
    else if(pid == -2)  // Last page number
        page = m_bookInfo->lastPage();
    else                // The given page number
        page = pid;

    m_quranFormat->clearQuranText();
    m_bookInfo->setCurrentPage(page);
    m_bookInfo->setCurrentSoraNumber(pid);

    m_bookQuery->exec(QString("SELECT QuranText.id, QuranText.ayaText, QuranText.ayaNumber, "
                              "QuranText.pageNumber, QuranText.soraNumber, QuranSowar.SoraName "
                              "FROM QuranText LEFT JOIN QuranSowar "
                              "ON QuranSowar.id = QuranText.soraNumber "
                              "WHERE QuranText.pageNumber = %1 "
                              "ORDER BY QuranText.id ").arg(page));

    while(m_bookQuery->next()) {
        // at the first vers we insert the sora name and bassemala
        if(m_bookQuery->value(2).toInt() == 1) {
            m_quranFormat->insertSoraName(m_bookQuery->value(5).toString());

            // we escape putting bassemala before Fateha and Tawba
            if(m_bookQuery->value(4).toInt() != 1 && m_bookQuery->value(4).toInt() != 9)
                m_quranFormat->insertBassemala();
        }
        m_quranFormat->insertAyaText(m_bookQuery->value(1).toString(),
                                     m_bookQuery->value(2).toInt(),
                                     m_bookQuery->value(4).toInt());
    }
    return m_quranFormat->text();
}

QString QuranDBHandler::openSora(int num)
{
        return page(getPageNumber(num));
}

QAbstractItemModel *QuranDBHandler::indexModel()
{
    BookIndexNode *rootNode = new BookIndexNode();

    m_bookQuery->exec("SELECT id, soraName FROM QuranSowar ORDER BY id");
    while(m_bookQuery->next()) {
        BookIndexNode *firstChild = new BookIndexNode(m_bookQuery->value(1).toString(),
                                                      m_bookQuery->value(0).toInt());
        rootNode->appendChild(firstChild);
    }

    m_indexModel->setRootNode(rootNode);
    return m_indexModel;
}

void QuranDBHandler::getBookInfo()
{
    m_bookQuery->exec(QString("SELECT  MIN(pageNumber), MAX(pageNumber), MIN(id), MAX(id) "
                              "FROM QuranText "));
    if(m_bookQuery->next()) {
        m_bookInfo->setPartsCount(1);
        m_bookInfo->setFirstPage(m_bookQuery->value(0).toInt());
        m_bookInfo->setLastPage(m_bookQuery->value(1).toInt());

        m_bookInfo->setFirstID(m_bookQuery->value(2).toInt());
        m_bookInfo->setLastID(m_bookQuery->value(3).toInt());
    }
}

int QuranDBHandler::getPageNumber(int soraNumber, int ayaNumber)
{
    int page = 1;
    m_bookQuery->exec(QString("SELECT pageNumber FROM QuranText WHERE soraNumber = %1 "
                              "AND ayaNumber = %2").arg(soraNumber).arg(ayaNumber));
    if(m_bookQuery->next()) {
        page = m_bookQuery->value(0).toInt();
        m_bookInfo->setCurrentAya(1);
        m_bookInfo->setCurrentSoraNumber(soraNumber);
    }
    return page;
}

QString QuranDBHandler::nextPage()
{
    return hasNext() ? page(m_bookInfo->currentPage()+1) : QString();
}

QString QuranDBHandler::prevPage()
{
    return hasPrev() ? page(m_bookInfo->currentPage()-1) : QString();
}

bool QuranDBHandler::hasNext()
{
    return m_bookInfo->currentPage() < m_bookInfo->lastPage();
}

bool QuranDBHandler::hasPrev()
{
    return m_bookInfo->currentPage() > m_bookInfo->firstPage();
}

QString QuranDBHandler::nextUnit()
{
//    int aya = m_bookInfo->currentAya()+1;
}

QString QuranDBHandler::prevUnit()
{

}
