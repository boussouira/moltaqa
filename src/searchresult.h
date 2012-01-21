#ifndef SEARCHRESULT_H
#define SEARCHRESULT_H

#include <qstring.h>
#include "librarybook.h"

class SearchResult
{
public:
    SearchResult(LibraryBook *_book, BookPage *_page);
    ~SearchResult();

    void generateHTML();
    QString toHtml();

    LibraryBook *book;
    BookPage *page;
    QString snippet;
    QString bgColor;
    int resultID;
    int score;

protected:
    QString m_html;
};

#endif // SEARCHRESULT_H
