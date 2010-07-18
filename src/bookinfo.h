#ifndef BOOKINFO_H
#define BOOKINFO_H

#include "pageinfo.h"
#include <QHash>

class BookInfo : public PageInfo
{
public:
    BookInfo();

    enum Type {
        QuranBook = 1,
        TafessirBook,
        HadditBook,
        NormalBook };

    int pagesCount(int part);
    int partsCount() { return m_partsCount; }
    int firstID() { return m_firstID; }
    int lastID() { return m_lastID; }
    int firstPage(int part=1);
    int lastPage(int part=1);
    QString bookName() { return m_bookName; }
    QString bookTable() { return m_bookTable; }
    QString titleTable() { return m_titleTable; }
    QString bookPath() { return m_bookPath; }
    BookInfo::Type bookType() const { return m_bookType; }
    int bookID() const { return m_bookID; }

    void setPagesCount(int count, int part=1);
    void setPartsCount(int count) { m_partsCount = count; }
    void setFirstID(int id) { m_firstID = id; }
    void setLastID(int id) { m_lastID = id; }
    void setFirstPage(int count, int part=1);
    void setLastPage(int count, int part=1);
    void setBookName(const QString &name) { m_bookName = name; }
    void setBookTable(const QString &table) { m_bookTable = table; }
    void setTitleTable(const QString &title) { m_titleTable = title; }
    void setBookPath(const QString &path) { m_bookPath = path; }
    void setBookType(BookInfo::Type type) { m_bookType = type; }
    void setBookID(int id) { m_bookID = id; }

    bool isQuran() { return m_bookType == QuranBook; }
    bool isNormal() { return m_bookType == NormalBook; }

protected:
    BookInfo::Type m_bookType;
    QString m_bookPath;
    QString m_bookName;
    QString m_bookTable;
    QString m_titleTable;
    QHash<int, int> m_firstPages;
    QHash<int, int> m_lastPages;
    int m_partsCount;
    int m_firstID;
    int m_lastID;
    int m_bookID;
};

#endif // BOOKINFO_H
