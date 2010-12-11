#ifndef PAGEINFO_H
#define PAGEINFO_H

#include <QString>

class PageInfo
{
public:
    PageInfo();
    void setCurrentSoraName(QString pSoraName);
    void setCurrentSoraDescent(QString pSoraDesc);
    void setCurrentSoraNumber(int pSoraNumber);
    void setCurrentAya(int pAyaNumber);
    void setCurrentSoraAyatCount(int pAyatCount);
    void setCurrentPage(int pPageNumber);
    void setCurrentPart(int part);
    void setCurrentID(int id);

    QString currentSoraName();
    QString currentSoraDescent();
    int currentSoraNumber();
    int currentAya() ;
    int currentSoraAyatCount();
    int currentPage();
    int currentID();
    int currentPart();

protected:
    QString m_currentSoraName ;
    QString m_currentSoraDescent;
    int m_currentSoraNumber;
    int m_currentAyaNumber;
    int m_currentSoraAyatCount ;
    int m_currentPageNumber;
    int m_currentID;
    int m_currentPart;

};

#endif // PAGEINFO_H
