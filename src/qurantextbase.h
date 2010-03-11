#ifndef QURANTEXTBASE_H
#define QURANTEXTBASE_H

#include <QObject>
#include "abstractqurantext.h"
#include "pageinfo.h"

class QuranTextBase : public AbstractQuranText
{
public:
    QuranTextBase(QObject *parent);
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
};

#endif // QURANTEXTBASE_H
