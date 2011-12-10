#ifndef PAGEINFO_H
#define PAGEINFO_H

#include <QString>

class BookPage
{
public:
    BookPage();

    int sora;   ///< Sora Number
    int aya;    ///< Aya number
    int ayatCount;  ///< Current sora ayat count
    int page;   ///< Page number
    int pageID; ///< Page id
    int titleID; ///< Title id
    int part;   ///< Part number
    int haddit; ///< Haddit number
    QString text; ///< Page Text
    QString title; ///< Page title

    BookPage *clone();
};

#endif // PAGEINFO_H
