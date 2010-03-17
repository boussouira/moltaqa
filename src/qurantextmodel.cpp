#include "qurantextmodel.h"

QuranTextModel::QuranTextModel() : QuranTextBase()
{
}

QString QuranTextModel::getQuranPage(PageInfo *pPageInfo)
{
    this->clearQuranText();

    m_quranQuery->prepare("SELECT QuranText.id, QuranText.ayaText, QuranText.ayaNumber, "
                     "QuranText.pageNumber, QuranText.soraNumber, QuranSowar.SoraName "
                     "FROM QuranText "
                     "LEFT JOIN QuranSowar "
                     "ON QuranSowar.id = QuranText.soraNumber "
                     "WHERE QuranText.pageNumber = :m_pageNumber "
                     "ORDER BY QuranText.id ");

    m_quranQuery->bindValue(":m_pageNumber", pPageInfo->currentPage());
    m_quranQuery->exec();

    while (m_quranQuery->next())
    {
        if(m_quranQuery->value(2).toInt() == 1) // at the first vers we insert the sora name and bassemala
        {
            this->insertSoraName(m_quranQuery->value(5).toString());
            // we escape putting bassemala before Fateha and Tawba
            if(pPageInfo->currentSoraNumber() != 1 and pPageInfo->currentSoraNumber() != 9)
            {
                this->insertBassemala();
            }
        }
        this->insertAyaText(m_quranQuery->value(1).toString(),
                                     m_quranQuery->value(2).toInt(),
                                     m_quranQuery->value(4).toInt());
    }

    return this->text();
}
