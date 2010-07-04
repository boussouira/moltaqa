#ifndef BOOKINFO_H
#define BOOKINFO_H

#include "pageinfo.h"
#include <QHash>

class BookInfo : public PageInfo
{
public:
    BookInfo();

    enum Type {
        QuranBook,
        TafessirBook,
        HadditBook,
        NormalBook };

    int pagesCount(int part);
    int partsCount() { return m_partsCount; }
    int firstID() { return m_firstID; }
    int lastID() { return m_lastID; }
    QString bookName() { return m_bookName; }
    QString bookTable() { return m_bookTable; }
    QString titleTable() { return m_titleTable; }
    BookInfo::Type bookType() const { return m_bookType; }

    void setPagesCount(int count, int part=1);
    void setPartsCount(int count) { m_partsCount = count; }
    void setFirstID(int id) { m_firstID = id; }
    void setLastID(int id) { m_lastID = id; }
    void setBookName(const QString &name) { m_bookName = name; }
    void setBookTable(const QString &table) { m_bookTable = table; }
    void setTitleTable(const QString &title) { m_titleTable = title; }
    void setBookType(BookInfo::Type type) { m_bookType = type; }

protected:
    BookInfo::Type m_bookType;
    QString m_bookName;
    QString m_bookTable;
    QString m_titleTable;
    QHash<int, int> m_pagesCount;
    int m_partsCount;
    int m_firstID;
    int m_lastID;
};

#endif // BOOKINFO_H
