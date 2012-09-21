#include "searchresult.h"
#include "htmlhelper.h"

SearchResult::SearchResult(LibraryBookPtr _book, BookPage *_page) :
    book(_book),
    page(_page)
{
    Q_CHECK_PTR(_book);
    Q_CHECK_PTR(_page);
}

SearchResult::~SearchResult()
{
    if(page)
        delete page;
}

QString SearchResult::toHtml()
{
    HtmlHelper helper;
    helper.beginDiv(".result");

    helper.beginDiv(".resultHead");
    helper.insertHtmlTag("h3", page->title);

    helper.beginSpan(".progSpan", QString("style=\"width: %1px;\"").arg(score));
    helper.insertSpan("", ".progSpanContainre");
    helper.endSpan();

    helper.endDiv(); //div.resultHead

    helper.beginDiv(".resultText", QString("bookid='%1' rid='%2'").arg(book->id).arg(resultID));
    helper.insertDiv(snippet, ".snippet");
    helper.endDiv();

    helper.beginDiv(".resultInfo", QString("bookid='%1'").arg(book->id));

    helper.beginDiv(".bookInfo");
    helper.insertSpan(QObject::tr("كتاب:"), ".book");
    helper.insertSpan(book->title, ".bookName");
    helper.endDiv();

    helper.beginDiv(".pageInfo");
    helper.insertSpan(QObject::tr("الصفحة:"), ".page");
    helper.insertSpan(QString::number(page->page), ".pageVal");
    helper.insertSpan(QObject::tr("الجزء:"), ".part");
    helper.insertSpan(QString::number(page->part), ".partVal");
    helper.endDiv();

    helper.insertDiv("", ".clear");

    helper.endAll();

    return helper.html();
}
