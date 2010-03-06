#ifndef SORAINFO_H
#define SORAINFO_H

#include <QString>

/**
  @brief Hold information about a SORA(it's name, number, AYAT count...)
  */
class SoraInfo
{
public:

    /// @brief The constructor.
    SoraInfo();

    /// @brief Change the current SORA name to the given one.
    /// @param pSoraName  The new SORA name.
    void setName(QString pSoraName)   {m_currentSoraName = pSoraName ;}

    /// @brief Change the SORA Descent to the given one(Mecca or Maddina).
    /// @param pSoraDesc  SORA descent.
    void setDescent(QString pSoraDesc){m_currentSoraDescent = pSoraDesc ;}

    /// @brief Change the current SORA number.
    /// @param pSoraNumber    The SORA number.
    void setNumber(int pSoraNumber)   {m_currentSoraNumber = pSoraNumber ;}

    /// @brief Change the current displayed AYA number.
    /// @param pAyaNumber     The current displayed AYA number.
    void setCurrentAya(int pAyaNumber) {m_currentAyaNumber = pAyaNumber ;}

    /// @brief Change the AYAT count of the current SORA.
    /// @param pAyatCount     Number of AYAT.
    void setAyatCount(int pAyatCount) {m_currentSoraAyatCount = pAyatCount ;}

    /// @brief    Change the current displayed page number.
    /// @param pPageNumber    The current displayed page number.
    void setCurrentPage(int pPageNumber){m_currentPageNumber = pPageNumber ;}


    /// @brief Get the current SORA name.
    /// @return Current SORA name.
    QString name() const { return m_currentSoraName; }

    /// @brief Get the current SORA descent.
    /// @return Current SORA descent.
    QString descent() const { return m_currentSoraDescent; }

    /// @brief Get the number of the current SORA.
    /// @return Current SORA number.
    int number() const { return m_currentSoraNumber; }

    /// @brief Get the current displayed AYA number.
    /// @return  Current displayed AYA number
    int currentAya() const { return m_currentAyaNumber; }

    /// @brief Get the current SORA AYAT count.
    /// @return AYAT number of the current SORA.
    int ayatCount() const { return m_currentSoraAyatCount; }

    /// @brief Get the current displayed page number.
    /// @return  Current displayed page number
    int currentPage() const { return m_currentPageNumber; }

private:
    QString m_currentSoraName ;
    QString m_currentSoraDescent;
    int m_currentSoraNumber;
    int m_currentAyaNumber;
    int m_currentSoraAyatCount ;
    int m_currentPageNumber;

};

#endif // SORAINFO_H
