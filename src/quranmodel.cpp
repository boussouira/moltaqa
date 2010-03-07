#include "quranmodel.h"
#include "pageinfo.h"
#include "ktext.h"

QuranModel::QuranModel(QObject *parent, QString pQuranDBPath) :
    QObject(parent), m_quranDBPath(pQuranDBPath)
{
    m_quranDB = QSqlDatabase::addDatabase("QSQLITE", "QuranDBModel");
    m_quranDB.setDatabaseName(m_quranDBPath);

    if (!m_quranDB.open()) {
        qDebug() << "Cannot open database.";
    }
    m_quranQuery = new QSqlQuery(m_quranDB);
    m_text = new KText();
}

void QuranModel::getSowarList(QStringListModel *pSowarModel)
{
    QStringList sowarNamesList;
    m_quranQuery->exec("SELECT * FROM QuranSowar");
    while (m_quranQuery->next())
    {
      sowarNamesList << QString("%1. %2")
              .arg(m_quranQuery->value(0).toString())
              .arg(m_quranQuery->value(1).toString() );
    }

    pSowarModel->setStringList(sowarNamesList);
}

void QuranModel::getPageInfo(int pSoraNumber, int pAyaNumber, PageInfo *pPageInfo)
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

int QuranModel::getAyaPageNumber(int pSoraNumber, int pAyaNumber)
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
QString QuranModel::getQuranPage(PageInfo *pPageInfo)
{
    m_text->clear();

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
            m_text->insertSoraName(m_quranQuery->value(5).toString());
            // we escape putting bassemala before Fateha and Tawba
            if(pPageInfo->currentSoraNumber() != 1 and pPageInfo->currentSoraNumber() != 9)
            {
                m_text->insertBassemala();
            }
        }
        m_text->insertAyaText(m_quranQuery->value(1).toString(),
                                     m_quranQuery->value(2).toInt(),
                                     m_quranQuery->value(4).toInt());
    }

    return m_text->text();
}

void QuranModel::getPageInfoByPage(int pPageNumber, PageInfo *pPageInfo)
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

int QuranModel::getFirsSoraNumberInPage(int pPageNumber)
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
