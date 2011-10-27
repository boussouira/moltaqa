#include "searchresult.h"

SearchResult::SearchResult(LibraryBook *_book, BookPage *_page)
{
    Q_ASSERT(_book);
    Q_ASSERT(_page);

    book = _book;
    page = _page;
}

SearchResult::~SearchResult()
{
}

void SearchResult::generateHTML()
{
    m_html.clear();

    m_html = QObject::tr("<div class=\"result %1\">"
                         "<div class=\"result_head\">"
                         "<h3>%2</h3>"
                         "<span class=\"progSpan\" style=\"width: %3px;\">"
                         "<span class=\"progSpanContainre\"></span>"
                         "</span>"
                         "</div>"
                         "<p class=\"resultText\" bookid=\"%4\" rid=\"%5\">%6</p>"
                         "<p class=\"result_info\" bookid=\"b%4\"> كتاب: <span class=\"bookName\">%7</span>"
                         "<span style=\"float: left;margin: 5px 0px\">الصفحة: <span style=\"margin-left: 7px;\">%8</span>  الجزء: <span>%9</span></span>"
                         "</p></div>")
            .arg(bgColor)               // backround class name (%1)
            .arg(title)                 // bab (%2)
            .arg(score)                 // score (%3)
            .arg(book->bookID)          // Book id (%4)
            .arg(resultID)              // Result id (%5)
            .arg(snippet.simplified())  // text snippet (%6)
            .arg(book->bookDisplayName) // book name (%7)
            .arg(page->page)            // page (%8)
            .arg(page->part);           // part (%9)

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
