#ifndef BOOKINFO_H
#define BOOKINFO_H

#include "pageinfo.h"
#include <qhash.h>

class BookInfo : public PageInfo
{
public:
    BookInfo();

    enum Type {
        QuranBook = 1,
        TafessirBook,
        NormalBook };

    int firstPage(int part=1);
    int lastPage(int part=1);

    void setPagesCount(int count, int part=1);
    void setFirstPage(int count, int part=1);
    void setLastPage(int count, int part=1);

    bool isQuran() { return bookType == QuranBook; }
    bool isNormal() { return bookType == NormalBook; }
    bool exists();

    QString toString();
    void fromString(QString info);
    bool haveInfo();

public:
    BookInfo::Type bookType;
    QString bookPath;
    QString bookName;
    QString textTable;
    QString indexTable;
    QString metaTable;
    QString tafessirTable;
    int partsCount;
    int firstID;
    int lastID;
    int bookID;

protected:
    QHash<int, int> m_firstPages;
    QHash<int, int> m_lastPages;
    bool m_hasInfo;
};

#endif // BOOKINFO_H
