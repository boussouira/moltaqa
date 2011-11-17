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

    QDir jsDir(App::jsDir());
    m_jqueryFile = QUrl::fromLocalFile(jsDir.filePath("jquery.js")).toString();
    m_scriptFile = QUrl::fromLocalFile(jsDir.filePath("scripts.js")).toString();
}

QString TextFormatter::getText()
{
    return m_html;
}

void TextFormatter::insertText(QString text)
{
    text.replace(QRegExp("[\\r\\n]"), "<br/>");
    m_html.append(text);
}

void TextFormatter::clearText()
{
    m_html.clear();
}

void TextFormatter::genHeaderAndFooter()
{
    if(m_book && m_page) {
        m_headerText = QString("<span class=\"bookName\">%1</span>"
                               "<span class=\"part\">%2</span>")
                .arg(m_book->bookDisplayName)
                .arg(m_page->part);

        m_footerText = QString("<span class=\"page\">%1</span>").arg(m_page->page);

        m_html.replace("<!--HEADER-->", m_headerText);
        m_html.replace("<!--FOOTER-->", m_footerText);
    }
}

void TextFormatter::insertHtmlTag(QString tag, QString text, QString className, QString idName)
{
    m_html.append(QString("<%1").arg(tag));

    if(!className.isEmpty())
        m_html.append(QString(" class=\"%1\"").arg(className));

    if(!idName.isEmpty())
        m_html.append(QString(" id=\"%1\"").arg(idName));

    m_html.append(QString(">%1</%2>").arg(text).arg(tag));

}

void TextFormatter::insertDivTag(QString text, QString className, QString idName)
{
    insertHtmlTag("div", text, className, idName);
}

void TextFormatter::insertSpanTag(QString text, QString className, QString idName)
{
    insertHtmlTag("span", text, className, idName);
}

void TextFormatter::insertImage(QString src)
{
    m_html.append("<img src=\"");
    m_html.append(src);
    m_html.append("\" />");
}

void TextFormatter::start()
{
    clearText();

    openHtmlTag("html");
    openHtmlTag("head");
    m_html.append("<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />");
    addCSS(m_styleFile);
    closeHtmlTag();

    openHtmlTag("body");
    openHtmlTag("div", QString(), m_cssID);

    insertDivTag("<!--HEADER-->", QString(), "pageHeader");

    openHtmlTag("div", QString(), "pageText");

    emit startReading();
}

void TextFormatter::done()
{
    closeHtmlTag(); // div.pageText

    insertDivTag("<!--FOOTER-->", QString(), "pageFooter");

    closeHtmlTag(); // div#m_cssID

    addJS(m_jqueryFile);
    addJS(m_scriptFile);

    closeAllTags();

    genHeaderAndFooter();

    emit doneReading();
}

void TextFormatter::openHtmlTag(QString tag, QString className, QString idName)
{
    m_html.append(QString("<%1").arg(tag));

    if(!className.isEmpty())
        m_html.append(QString(" class=\"%1\"").arg(className));

    if(!idName.isEmpty())
        m_html.append(QString(" id=\"%1\"").arg(idName));

    m_html.append(">");

    m_openTags.push(tag);
}

void TextFormatter::closeHtmlTag(QString tag)
{
    if(tag.isEmpty()) {
        if(!m_openTags.isEmpty())
            m_html.append(QString("</%1>").arg(m_openTags.pop()));
    } else {
        m_html.append(QString("</%1>").arg(tag));
    }
}

void TextFormatter::closeAllTags()
{
    while(!m_openTags.isEmpty())
        m_html.append(QString("</%1>").arg(m_openTags.pop()));
}

void TextFormatter::addCSS(QString cssFile)
{
    m_html.append(QString("<link href= \"%1\" rel=\"stylesheet\" type=\"text/css\" />").arg(cssFile));
}

void TextFormatter::addJS(QString jsFile)
{
    m_html.append(QString("<script type=\"text/javascript\" src=\"%1\"></script>").arg(jsFile));
}
