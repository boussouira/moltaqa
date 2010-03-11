#include "qurantextbase.h"

QuranTextBase::QuranTextBase(QObject *parent) : AbstractQuranText(parent)
{
}

void QuranTextBase::getPageInfo(int pSoraNumber, int pAyaNumber, PageInfo *pPageInfo)
{
    pPageInfo->setCurrentSoraNumber(pSoraNumber);

    m_quranQuery->prepare("SELECT SoraName, ayatCount, SoraDescent "
                     "FROM QuranSowar "
                     "WHERE QuranSowar.id = :id LIMIT 1");

    m_quranQuery->bindValue(":id", pPageInfo->currentSoraNumber());
    m_quranQuery->exec();

    if (m_quranQuery->first()){
        pPageInfo->setCurrentSoraName(m_quranQuery->value(0).toString());
        pPageInfo->setCurrentSoraAyatCount(m_quranQuery->value(1).toInt());
        pPageInfo->setCurrentSoraDescent(m_quranQuery->value(2).toString());
    }

    if (pAyaNumber <= pPageInfo->currentSoraAyatCount())
        pPageInfo->setCurrentAya(pAyaNumber);
     else
        pPageInfo->setCurrentAya(1);

    int ayaPage = getAyaPageNumber(pPageInfo->currentSoraNumber(), pPageInfo->currentAya());
    pPageInfo->setCurrentPage(ayaPage);

}

int QuranTextBase::getAyaPageNumber(int pSoraNumber, int pAyaNumber)
{
    m_quranQuery->prepare("SELECT QuranText.pageNumber "
                          "FROM QuranText "
                          "WHERE QuranText.soraNumber = :s_soraNumber "
                          "AND QuranText.ayaNumber = :s_ayaNumber "
                          "LIMIT 1 ");

    m_quranQuery->bindValue(":s_soraNumber", pSoraNumber);
    m_quranQuery->bindValue(":s_ayaNumber", pAyaNumber);
    m_quranQuery->exec();

    if(m_quranQuery->first())
        return m_quranQuery->value(0).toInt();
    else
        return 1;
}

void QuranTextBase::getPageInfoByPage(int pPageNumber, PageInfo *pPageInfo)
{
    int soraNumber = getFirsSoraNumberInPage(pPageNumber);
    m_quranQuery->prepare("SELECT MIN(QuranText.ayaNumber) AS FirstAyaInPage "
                     "FROM QuranText "
                     "WHERE QuranText.soraNumber = :s_soraNumber "
                     "AND QuranText.pageNumber = :s_pageNumber "
                     "LIMIT 1 ");

    m_quranQuery->bindValue(":s_soraNumber", soraNumber);
    m_quranQuery->bindValue(":s_pageNumber", pPageNumber);
    m_quranQuery->exec();

    if(m_quranQuery->first()) {
        pPageInfo->setCurrentSoraNumber(soraNumber);
        pPageInfo->setCurrentPage(pPageNumber);
        //The first AYA in the page
        pPageInfo->setCurrentAya(m_quranQuery->value(0).toInt());

    }
}

int QuranTextBase::getFirsSoraNumberInPage(int pPageNumber)
{
    m_quranQuery->prepare("SELECT MIN(QuranText.soraNumber) AS FirstSoraInPage "
                     "FROM QuranText "
                     "WHERE QuranText.pageNumber = :s_pageNumber "
                     "LIMIT 1 ");
    m_quranQuery->bindValue(":s_pageNumber", pPageNumber);
    m_quranQuery->exec();

    if(m_quranQuery->first()) {
        // The first SORA in the page
        return m_quranQuery->value(0).toInt();
    }
    return 0;
}
