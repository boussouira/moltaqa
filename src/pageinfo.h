#ifndef PAGEINFO_H
#define PAGEINFO_H

#include <QString>

class PageInfo
{
public:
    PageInfo();

    int sora;   ///< Sora Number
    int aya;    ///< Aya number
    int ayatCount;  ///< Current sora ayat count
    int page;   ///< Page number
    int pageID; ///< Page id
    int part;   ///< Part number
    int haddit; ///< Haddit number
};

#endif // PAGEINFO_H
