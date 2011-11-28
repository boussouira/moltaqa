#include "searchresult.h"

SearchResult::SearchResult(LibraryBook *_book, BookPage *_page) :
    book(_book),
    page(_page)
{
    Q_CHECK_PTR(_book);
    Q_CHECK_PTR(_page);
}

SearchResult::~SearchResult()
{
    if(book)
        delete book;

    if(page)
        delete page;
}

void SearchResult::generateHTML()
{
    m_html = QObject::tr
            (
            "<div class=\"result\">"
                "<div class=\"resultHead\">"
                    "<h3>%1</h3>"
                    "<span class=\"progSpan\" style=\"width: %2px;\">"
                        "<span class=\"progSpanContainre\"></span>"
                    "</span>"
                "</div>"
                "<div class=\"resultText\" bookid=\"%3\" rid=\"%4\">%5</div>"
                "<div class=\"resultInfo\" bookid=\"b%3\">"
                    "<div class=\"bookInfo\">"
                        "<span class=\"book\">كتاب:</span>"
                        "<span class=\"bookName\">%6</span>"
                    "</div>"
                    "<div class=\"pageInfo\">"
                        "<span class=\"page\">الصفحة:</span><span class=\"pageVal\">%7</span>"
                        "<span class=\"part\">الجزء:</span><span class=\"partVal\">%8</span>"
                    "</div>"
                    "<div class=\"clear\">"
                    "</div>"
                "</div>"
            "</div>"
             ) // Nice formating :)
            .arg(page->title)           // bab (%1)
            .arg(score)                 // score (%2)
            .arg(book->bookID)          // Book id (%3)
            .arg(resultID)              // Result id (%4)
            .arg(snippet.simplified())  // text snippet (%5)
            .arg(book->bookDisplayName) // book name (%6)
            .arg(page->page)            // page (%7)
            .arg(page->part);           // part (%8)

}

QString SearchResult::toHtml()
{
    return m_html;
}

QDebug& operator <<(QDebug &dbg, const SearchResult &result)
{
    dbg.nospace() << "{\n    Book: " << result.book->bookDisplayName << ",\n    "
             << "Page: " << result.page->page << ",\n    "
             << "Part: " << result.page->part << ",\n    "
             << "Score: " << result.score << ",\n    "
             << "Text size: " << result.snippet.size()
             << "\n}";

    return dbg.space();
}
