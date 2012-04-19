#include "textformatter.h"
#include "utils.h"
#include "librarybook.h"
#include "bookpage.h"
#include <qurl.h>
#include <qapplication.h>
#include <qdir.h>
#include <qdebug.h>

TextFormatter::TextFormatter(QObject *parent): QObject(parent)
{
    laodSettings();

    m_page = 0;
}

void TextFormatter::setData(LibraryBookPtr book, BookPage *page)
{
    m_book = book;
    m_page = page;
}

void TextFormatter::laodSettings()
{
    QDir styleDir(App::stylesDir());
    styleDir.cd("default");

    QString style = styleDir.filePath("default.css");
    m_styleFile = QUrl::fromLocalFile(style).toString();
}

QString TextFormatter::getText()
{
    return m_htmlHelper.html();
}

void TextFormatter::insertText(QString text)
{
    m_htmlHelper.append(text);
}

QString TextFormatter::getHtmlView(const QString &text, const QString &jsCode)
{
    ML_ASSERT_RET2(m_book, "TextFormatter::getHtmlView book is null", QString());
    ML_ASSERT_RET2(m_page, "TextFormatter::getHtmlView page is null", QString());

    HtmlHelper helper;
    helper.beginHtml();

    helper.beginHead();
    helper.setCharset("utf-8");
    helper.addCSS(m_styleFile);

    helper.endHead();

    helper.beginBody();
    helper.beginDiv("#" + m_cssID);

    if(!m_book->isQuran()) {
        helper.beginDiv("#pageHeader");

        helper.beginDiv(".bookInfo");
        helper.insertSpan(m_book->title, ".bookName");

        helper.beginSpan("#partInfo");
        helper.insertSpan(tr("الجزء"), ".partText");
        helper.insertSpan(QString::number(m_page->part), ".partNum");
        helper.endSpan(); // span#partInfo

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

    if(!m_book->isQuran()) {
        helper.beginDiv("#pageFooter");
        helper.insertSpan(QString::number(m_page->page), ".page");
        helper.endDiv(); // div#pageFooter
    }

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
        qDebug("TextFormatter: no page to save text");

    emit doneReading();
}
