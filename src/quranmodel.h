#ifndef KMODEL_H
#define KMODEL_H

#include <QObject>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringListModel>

class PageInfo;
class KText;

/**
  @brief Extract information and text from the Quran database.
  */
class QuranModel : public QObject
{
    Q_OBJECT
public:

    /**
      @brief The constructor.
      @param parent         The parent widget.
      @param pQuranDBPath   The Quran database path.
      */
    explicit QuranModel(QObject *parent, QString pQuranDBPath);
public slots:

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

    /**
      @brief Get formated Quran text of the given PageInfo from the database.
      @param pPageInfo      Page information(SORA, AYA and page...)
      @return Formatted text.
      @see KText::text()
      */
    QString getQuranPage(PageInfo *pPageInfo);

    /**
      @brief Get information about the first SORA in the given page number.
      @param pPageNumber        The page number.
      @param pPageInfo          Pointer to a PageInfo which well be fill in with the information.
      @see getFirsSoraNumberInPage
      */
    void getPageInfoByPage(int pPageNumber, PageInfo *pPageInfo);

    /**
      @brief Get the first SORA number in the given page number.
      @param pPageNumber        The page number.
      @return The first SORA number in the given page.
      */
    int getFirsSoraNumberInPage(int pPageNumber);

private:
    KText *m_text;
    QSqlDatabase m_quranDB;
    QSqlQuery *m_quranQuery;
    QString m_quranDBPath;

};

#endif // KMODEL_H

