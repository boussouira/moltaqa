#ifndef PAGEINFO_H
#define PAGEINFO_H

#include <QString>

/// @brief This class hold information about pages.
class PageInfo
{
public:
    PageInfo();

    /// @brief Change the current SORA name to the given one.
    /// @param pSoraName  The new SORA name.
    void setCurrentSoraName(QString pSoraName)   {m_currentSoraName = pSoraName ;}

    /// @brief Change the SORA descent to the given one(Mecca or Maddina).
    /// @param pSoraDesc  SORA descent.
    void setCurrentSoraDescent(QString pSoraDesc){m_currentSoraDescent = pSoraDesc ;}

    /// @brief Change the current SORA number.
    /// @param pSoraNumber    The SORA number.
    void setCurrentSoraNumber(int pSoraNumber)   {m_currentSoraNumber = pSoraNumber ;}

    /// @brief Change the current displayed AYA number.
    /// @param pAyaNumber     The current displayed AYA number.
    void setCurrentAya(int pAyaNumber) {m_currentAyaNumber = pAyaNumber ;}

    /// @brief Change the AYAT count of the current SORA.
    /// @param pAyatCount     Number of AYAT.
    void setCurrentSoraAyatCount(int pAyatCount) {m_currentSoraAyatCount = pAyatCount ;}

    /// @brief    Change the current displayed page number.
    /// @param pPageNumber    The current displayed page number.
    void setCurrentPage(int pPageNumber){m_currentPageNumber = pPageNumber ;}


    /// @brief Set The current page ID.
    void setCurrentID(int id) { m_currentID = id; }

    /// @brief Get the current SORA name.
    /// @return Current SORA name.
    QString currentSoraName() const { return m_currentSoraName; }

    /// @brief Get the current SORA descent.
    /// @return Current SORA descent.
    QString currentSoraDescent() const { return m_currentSoraDescent; }

    /// @brief Get the number of the current SORA.
    /// @return Current SORA number.
    int currentSoraNumber() const { return m_currentSoraNumber; }

    /// @brief Get the current displayed AYA number.
    /// @return  Current displayed AYA number
    int currentAya() const { return m_currentAyaNumber; }

    /// @brief Get the current SORA AYAT count.
    /// @return AYAT number of the current SORA.
    int currentSoraAyatCount() const { return m_currentSoraAyatCount; }

    /// @brief Get the current displayed page number.
    /// @return  Current displayed page number
    int currentPage() const { return m_currentPageNumber; }

    int currentID() const { return m_currentID; }
    /// @brief Get the current page Type.

protected:
    QString m_currentSoraName ;
    QString m_currentSoraDescent;
    int m_currentSoraNumber;
    int m_currentAyaNumber;
    int m_currentSoraAyatCount ;
    int m_currentPageNumber;
    int m_currentID;

};

#endif // PAGEINFO_H
