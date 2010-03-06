#ifndef KMODEL_H
#define KMODEL_H

#include <QObject>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringListModel>

class SoraInfo;
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
    @param pSoraInfo        Pointer to SoraInfo.
      */
    void getSoraInfo(int pSoraNumber, int pAyaNumber, SoraInfo *pSoraInfo);

    /**
      @brief Get the page number of a SORA and AYA number.
      @param pSoraNumber    The SORA number.
      @param pAyaNumber     The AYA number.
      @return The page number.
      */
    int getAyaPageNumber(int pSoraNumber, int pAyaNumber);

    /**
      @brief Get formated Quran text of the given SoraInfo from the database.
      @param pSoraInfo      Page information(SORA, AYA and page...)
      @return Formatted text.
      @see KText::text()
      */
    QString getQuranPage(SoraInfo *pSoraInfo);

    /**
      @brief Get information about the first SORA in the given page number.
      @param pPageNumber        The page number.
      @param pSoraInfo          Pointer to a SoraInfo which well be fill in with the information.
      @see getFirsSoraNumberInPage
      */
    void getSoraInfoByPage(int pPageNumber, SoraInfo *pSoraInfo);

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

