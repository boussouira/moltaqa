#include "searchresult.h"
#include "htmlhelper.h"
#include "utils.h"

SearchResult::SearchResult(LibraryBook::Ptr _book, BookPage *_page) :
    book(_book),
    page(_page)
{
    Q_CHECK_PTR(_book);
    Q_CHECK_PTR(_page);
}

SearchResult::~SearchResult()
{
    ml_delete_check(page);
}

QString SearchResult::toHtml()
{
    ml_return_val_on_fail(page, QString());

    if(m_html.size())
        return m_html;

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
    if(page->page) {
        helper.insertSpan(QObject::tr("الصفحة:"), ".page");
        helper.insertSpan(QString::number(page->page), ".pageVal");
    }

    if(page->part) {
        helper.insertSpan(QObject::tr("الجزء:"), ".part");
        helper.insertSpan(QString::number(page->part), ".partVal");
    }

    helper.endDiv();

    helper.insertDiv("", ".clear");

    helper.endAll();

    m_html = helper.html();

    return m_html;
}
