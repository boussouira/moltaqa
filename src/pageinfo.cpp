#include "pageinfo.h"

PageInfo::PageInfo()
{
    m_currentSoraNumber = 0;
    m_currentAyaNumber = 0;
    m_currentSoraAyatCount  = 0;
    m_currentPageNumber = 0;
    m_currentID = 0;
    m_currentPart = 0;
}

void PageInfo::setCurrentSoraName(QString pSoraName)
{
    m_currentSoraName = pSoraName;
}

void PageInfo::setCurrentSoraDescent(QString pSoraDesc)
{
    m_currentSoraDescent = pSoraDesc;
}

void PageInfo::setCurrentSoraNumber(int pSoraNumber)
{
    m_currentSoraNumber = pSoraNumber;
}

void PageInfo::setCurrentAya(int pAyaNumber)
{
    m_currentAyaNumber = pAyaNumber;
}

void PageInfo::setCurrentSoraAyatCount(int pAyatCount)
{
    m_currentSoraAyatCount = pAyatCount;
}

void PageInfo::setCurrentPage(int pPageNumber)
{
    m_currentPageNumber = pPageNumber;
}

void PageInfo::setCurrentPart(int part)
{
    m_currentPart = part;
}

void PageInfo::setCurrentID(int id)
{
    m_currentID = id;
}

QString PageInfo::currentSoraName()
{
    return m_currentSoraName;
}

QString PageInfo::currentSoraDescent()
{
    return m_currentSoraDescent;
}

int PageInfo::currentSoraNumber()
{
    return m_currentSoraNumber;
}

int PageInfo::currentAya()
{
    return m_currentAyaNumber;
}

int PageInfo::currentSoraAyatCount()
{
    return m_currentSoraAyatCount;
}

int PageInfo::currentPage()
{
    return m_currentPageNumber;
}

int PageInfo::currentID()
{
    return m_currentID;
}

int PageInfo::currentPart()
{
    return m_currentPart;
}
