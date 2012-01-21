#include "searchresult.h"
#include "htmlhelper.h"

SearchResult::SearchResult(LibraryBook *_book, BookPage *_page) :
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

void SearchResult::generateHTML()
{
    HtmlHelper helper;
    helper.beginDivTag(".result");

    helper.beginDivTag(".resultHead");
    helper.insertHtmlTag("h3", page->title);

    helper.beginSpanTag(".progSpan", QString("style=\"width: %1px;\"").arg(score));
    helper.insertSpanTag("", ".progSpanContainre");
    helper.endSpanTag();

    helper.endDivTag(); //div.resultHead

    helper.beginDivTag(".resultText", QString("bookid='%1' rid='%2'").arg(book->bookID).arg(resultID));
    helper.append(snippet.simplified());
    helper.endDivTag();

    helper.beginDivTag(".resultInfo", QString("bookid='b%1'").arg(book->bookID));

    helper.beginDivTag(".bookInfo");
    helper.insertSpanTag(QObject::tr("كتاب:"), ".book");
    helper.insertSpanTag(book->bookDisplayName, ".bookName");
    helper.endDivTag();

    helper.beginDivTag(".pageInfo");
    helper.insertSpanTag(QObject::tr("الصفحة:"), ".page");
    helper.insertSpanTag(QString::number(page->page), ".pageVal");
    helper.insertSpanTag(QObject::tr("الجزء:"), ".part");
    helper.insertSpanTag(QString::number(page->part), ".partVal");
    helper.endDivTag();

    helper.insertDivTag("", ".clear");

    helper.endAllTags();

    m_html = helper.html();
}

QString SearchResult::toHtml()
{
    return m_html;
}
