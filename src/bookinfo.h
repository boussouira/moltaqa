#ifndef BOOKINFO_H
#define BOOKINFO_H

#include "pageinfo.h"
#include <QHash>

class BookInfo : public PageInfo
{
public:
    BookInfo();
    int pagesCount(int part);
    int partsCount() { return m_partsCount; }
    QString bookName() { return m_bookName; }
    QString bookTable() { return m_bookTable; }
    QString titleTable() { return m_titleTable; }

    void setPagesCount(int count, int part=1);
    void setPartsCount(int count) { m_partsCount = count; }
    void setBookName(const QString &name) { m_bookName = name; }
    void setBookTable(const QString &table) { m_bookTable = table; }
    void setTitleTable(const QString &title) { m_titleTable = title; }

protected:
    QString m_bookName;
    QString m_bookTable;
    QString m_titleTable;
    QHash<int, int> m_pagesCount;
    int m_partsCount;
};

#endif // BOOKINFO_H
