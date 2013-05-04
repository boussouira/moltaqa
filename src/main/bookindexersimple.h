#ifndef BOOKINDEXERSIMPLE_H
#define BOOKINDEXERSIMPLE_H

#include "bookindexerbase.h"
#include "quazip.h"
#include "quazipfile.h"

class BookIndexerSimple : public BookIndexerBase
{
public:
    BookIndexerSimple();
    ~BookIndexerSimple();

    void open();
    void start();

protected:
    void loadTitles();
    void morePageIndex(BookPage *page);

protected:
    QuaZip m_zip;
    QList<int> m_titles;
    QHash<int, QString> m_titlesText;
    wchar_t *m_authorDeath;
    bool m_unknowAuthor;
    int m_lastTitleID;
};

#endif // BOOKINDEXERSIMPLE_H
