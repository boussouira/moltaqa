#ifndef QURANTEXTBASE_H
#define QURANTEXTBASE_H

#include <QObject>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringListModel>
#include "pageinfo.h"

class QuranTextBase
{
public:
    QuranTextBase();

    /**
      @brief Open the Quran database.
      @param pQuranDBPath   Quran database path.
      */
    void openQuranDB(QString pQuranDBPath);

    /**
      @brief Get all SOWAR names as QStringListModel.
      @param pSowarModel    Pointer to a QStringListModel.
      */
    void getSowarList(QStringListModel *pSowarModel);

    /**
    @brief Get information about a SORA.
    @param pSoraNumber      The SORA number.
    @param pAyaNumber       AYA number
    @param pPageInfo        Pointer to PageInfo.
      */
    void getPageInfo(int pSoraNumber, int pAyaNumber, PageInfo *pPageInfo);

    /**
      @brief Get the page number of a SORA and AYA number.
      @param pSoraNumber    The SORA number.
      @param pAyaNumber     The AYA number.
      @return The page number.
      */
    int getAyaPageNumber(int pSoraNumber, int pAyaNumber);
    void getPageInfoByPage(int pPageNumber, PageInfo *pPageInfo);
    /**
      @brief Get the first SORA number in the given page number.
      @param pPageNumber        The page number.
      @return The first SORA number in the given page.
      */
    int getFirsSoraNumberInPage(int pPageNumber);

protected:
    QSqlDatabase m_quranDB;
    QSqlQuery *m_quranQuery;
    QString m_quranDBPath;
};

#endif // QURANTEXTBASE_H
