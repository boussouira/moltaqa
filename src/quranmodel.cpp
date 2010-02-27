#include "quranmodel.h"
#include "sorainfo.h"
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
QuranModel::~QuranModel()
{
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

void QuranModel::getSoraInfo(int pSoraNumber, int pAyaNumber, SoraInfo *pSoraInfo)
{
    pSoraInfo->setNumber(pSoraNumber);

    m_quranQuery->prepare("SELECT SoraName, ayatCount, SoraDescent "
                     "FROM QuranSowar "
                     "WHERE QuranSowar.id = :id LIMIT 1");

    m_quranQuery->bindValue(":id", pSoraInfo->number());
    m_quranQuery->exec();

    if (m_quranQuery->first()){
        pSoraInfo->setName(m_quranQuery->value(0).toString());
        pSoraInfo->setAyatCount(m_quranQuery->value(1).toInt());
        pSoraInfo->setDescent(m_quranQuery->value(2).toString());
    }

    m_quranQuery->prepare("SELECT MIN(QuranText.pageNumber) AS MinPNum, MAX(QuranText.pageNumber) AS MaxPNum "
                     "FROM QuranText "
                     "WHERE QuranText.soraNumber = :m_currentSoraNumber ");

    m_quranQuery->bindValue(":m_currentSoraNumber", pSoraInfo->number());
    m_quranQuery->exec();

    if(m_quranQuery->first())
    {
        pSoraInfo->setCurrentPage(m_quranQuery->value(0).toInt());
    }
    if (pAyaNumber <= pSoraInfo->ayatCount())
        pSoraInfo->setCurrentAya(pAyaNumber);
     else
        pSoraInfo->setCurrentAya(1);

    int ayaPage = getAyaPageNumber(pSoraInfo->number(), pSoraInfo->currentAya());
    pSoraInfo->setCurrentPage(ayaPage);

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
QString QuranModel::getQuranPage(SoraInfo *pSoraInfo)
{
    m_text->clear();

    m_quranQuery->prepare("SELECT QuranText.id, QuranText.ayaText, QuranText.ayaNumber, "
                     "QuranText.pageNumber, QuranText.soraNumber, QuranSowar.SoraName "
                     "FROM QuranText "
                     "LEFT JOIN QuranSowar "
                     "ON QuranSowar.id = QuranText.soraNumber "
                     "WHERE QuranText.pageNumber = :m_pageNumber "
                     "ORDER BY QuranText.id ");

    m_quranQuery->bindValue(":m_pageNumber", pSoraInfo->currentPage());
    m_quranQuery->exec();

    while (m_quranQuery->next())
    {
        if(m_quranQuery->value(2).toInt() == 1) // at the first vers we insert the sora name and bassemala
        {
            m_text->insertSoraName(m_quranQuery->value(5).toString());
            if(pSoraInfo->number() != 1 and pSoraInfo->number() != 9) // we escape putting bassemala before Fateha and Tawba
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


