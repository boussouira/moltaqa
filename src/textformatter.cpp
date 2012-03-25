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

    m_book = 0;
    m_page = 0;
}

void TextFormatter::setData(LibraryBook *book, BookPage *page)
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

QString TextFormatter::getHtmlView(QString text)
{
    Q_CHECK_PTR(m_book);
    Q_CHECK_PTR(m_page);

    HtmlHelper helper;
    helper.beginHtml();

    helper.beginHead();
    helper.setCharset("utf-8");
    helper.addCSS(m_styleFile);

    helper.addJSCode(QString("var PAGE_ID = '%1';").arg(m_cssID));
    helper.addJSCode(QString("var BOOK_NAME = '%1';").arg(m_book->bookDisplayName));

    helper.endHead();

    helper.beginBody();
    helper.beginDivTag("#" + m_cssID);

    if(!m_book->isQuran()) {
        helper.beginDivTag("#pageHeader");

        helper.beginDivTag(".bookInfo");
        helper.insertSpanTag(m_book->bookDisplayName, ".bookName");

        helper.beginSpanTag("#partInfo");
        helper.insertSpanTag(tr("الجزء"), ".partText");
        helper.insertSpanTag(QString::number(m_page->part), ".partNum");
        helper.endSpanTag(); // span#partInfo

        helper.endDivTag(); // div.bookInfo

        helper.beginDivTag(".breadcrumbs");
        helper.endDivTag();

        helper.endDivTag(); // div#pageHeader
    }

    if(m_book->isNormal()) {
        helper.beginDivTag("#shorooh");
        helper.insertSpanTag(tr("الشروح (0)"), ".info");

        helper.insertDivTag("", ".clear");
        helper.insertDivTag("", ".shoroohBooks");

        helper.endDivTag(); // div#shorooh
    }

    helper.insertDivTag(text, "#pageText");

    if(!m_book->isQuran()) {
        helper.beginDivTag("#pageFooter");
        helper.insertSpanTag(QString::number(m_page->page), ".page");
        helper.endDivTag(); // div#pageFooter
    }

    helper.endDivTag(); // div#m_cssID

    QDir jsDir(App::jsDir());
    helper.addJS(QUrl::fromLocalFile(jsDir.filePath("jquery.js")).toString());
    helper.addJS(QUrl::fromLocalFile(jsDir.filePath("jquery.tooltip.js")).toString());
    helper.addJS(QUrl::fromLocalFile(jsDir.filePath("scripts.js")).toString());

    helper.addJSCode("pageTextChanged();");

    if(m_book->isNormal())
        helper.addJSCode("toggleShorooh();");

    helper.endAllTags();

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
    m_htmlHelper.endAllTags();

    if(m_page)
        m_page->text = m_htmlHelper.html();
    else
        qDebug("TextFormatter: no page to save text");

    emit doneReading();
}
