#include "textformatter.h"
#include "bookpage.h"
#include "librarybook.h"
#include "utils.h"

#include <qapplication.h>
#include <qdir.h>
#include <qurl.h>

TextFormatter::TextFormatter(QObject *parent): QObject(parent)
{
    laodSettings();

    m_page = 0;
}

void TextFormatter::setData(LibraryBook::Ptr book, BookPage *page)
{
    m_book = book;
    m_page = page;
}

void TextFormatter::laodSettings()
{
}

QString TextFormatter::getText()
{
    return m_htmlHelper.html();
}

void TextFormatter::insertText(QString text)
{
    m_htmlHelper.appendText(text);
}

QString TextFormatter::getHtmlView(const QString &text, const QString &jsCode)
{
    ml_return_val_on_fail2(m_book, "TextFormatter::getHtmlView book is null", QString());
    ml_return_val_on_fail2(m_page, "TextFormatter::getHtmlView page is null", QString());

    HtmlHelper helper;
    helper.beginHtml();

    helper.beginHead();
    helper.setCharset("utf-8");
    helper.addCSS("default.css");

    helper.endHead();

    helper.beginBody();
    helper.beginDiv("#" + m_cssID);

    if(!m_book->isQuran()) {
        helper.beginDiv("#pageHeader");

        helper.beginDiv(".bookInfo");
        helper.insertSpan(m_book->title, ".bookName");

        helper.beginDiv("#currentPage");

        helper.beginSpan(".page");
        helper.insertSpan(tr("الصفحة "), ".name");
        helper.insertSpan(QString::number(m_page->page), ".val");
        helper.endSpan(); // span.page

        helper.beginSpan(".part");
        helper.insertSpan(tr(" الجزء "), ".name");
        helper.insertSpan(QString::number(m_page->part), ".val");
        helper.endSpan(); // span.part

        helper.endDiv(); // div#currentPage

        helper.endDiv(); // div.bookInfo

        helper.beginDiv(".breadcrumbs");
        helper.endDiv();

        helper.endDiv(); // div#pageHeader
    }

    if(m_book->isNormal()) {
        helper.beginDiv("#shorooh");
        helper.insertSpan(tr("الشروح (0)"), ".info");

        helper.insertDiv("", ".clear");
        helper.insertDiv("", ".shoroohBooks");

        helper.endDiv(); // div#shorooh
    }

    helper.insertDiv(text, "#pageText");

    helper.endDiv(); // div#m_cssID

    helper.addJS("jquery.js");
    helper.addJS("jquery.tooltip.js");
    helper.addJS("scripts.js");
    helper.addJS("reader.js");

    helper.addJSCode("pageTextChanged();");

    if(m_book->isNormal())
        helper.addJSCode("toggleShorooh();");

    helper.addJSCode(jsCode);

    helper.endAll();

    return helper.html();
}

void TextFormatter::clearText()
{
    m_htmlHelper.clear();
}

void TextFormatter::start()
{
    clearText();

    emit startReading();
}

void TextFormatter::done()
{
    m_htmlHelper.endAll();

    if(m_page)
        m_page->text = m_htmlHelper.html();
    else
        qDebug("TextFormatter::done no page to save text");

    emit doneReading();
}
