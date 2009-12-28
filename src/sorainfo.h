#ifndef SORAINFO_H
#define SORAINFO_H

#include <QString>

class SoraInfo
{
public:
    SoraInfo();
    void setName(QString pSoraName)   {m_currentSoraName = pSoraName ;}
    void setDescent(QString pSoraDesc){m_currentSoraDescent = pSoraDesc ;}
    void setNumber(int pSoraNumber)   {m_currentSoraNumber = pSoraNumber ;}
    void setCurrentAya(int pAyaNumber) {m_currentAyaNumber = pAyaNumber ;}
    void setAyatCount(int pAyatCount) {m_currentSoraAyatCount = pAyatCount ;}
    void setCurrentPage(int pPageNumber){m_currentPageNumber = pPageNumber ;}

    QString name() const { return m_currentSoraName; }
    QString descent() const { return m_currentSoraDescent; }
    int number() const { return m_currentSoraNumber; }
    int currentAya() const { return m_currentAyaNumber; }
    int ayatCount() const { return m_currentSoraAyatCount; }
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
