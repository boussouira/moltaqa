#include "textformatter.h"
#include <qurl.h>
#include <qapplication.h>
#include <qdir.h>
#include <qdebug.h>

TextFormatter::TextFormatter(QObject *parent): QObject(parent)
{
    laodSettings();
}

void TextFormatter::laodSettings()
{
    QDir dir(QApplication::applicationDirPath());
    dir.cd("styles");
    dir.cd("default");
    QString style = dir.filePath("default.css");
    m_styleFile = QUrl::fromLocalFile(style).toString();
}

QString TextFormatter::getText()
{
    return m_html;
}

void TextFormatter::insertText(QString text)
{
    text.replace(QRegExp("[\\r\\n]"), "<br/>");
    m_text.append(text);
}

void TextFormatter::clearText()
{
    m_text.clear();
    m_html.clear();
}

void TextFormatter::insertHtmlTag(QString tag, QString text, QString className, QString idName)
{
    m_text.append(QString("<%1").arg(tag));

    if(!className.isEmpty())
        m_text.append(QString(" class=\"%1\"").arg(className));

    if(!idName.isEmpty())
        m_text.append(QString(" id=\"%1\"").arg(idName));

    m_text.append(QString(">%1</%2>").arg(text).arg(tag));

}

void TextFormatter::insertDivTag(QString text, QString className, QString idName)
{
    insertHtmlTag("div", text, className, idName);
}

void TextFormatter::insertSpanTag(QString text, QString className, QString idName)
{
    insertHtmlTag("span", text, className, idName);
}

void TextFormatter::start(bool clear)
{
    if(clear)
        clearText();

    emit startReading();
}

void TextFormatter::done()
{
    m_html = QString("<html>"
                     "<head>"
                     "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />"
                     "<link href= \"%1\" rel=\"stylesheet\" type=\"text/css\"/>"
                     "</head>"
                     "<body>"
                     "<div id=\"%2\">%3</div>"
                     "</body></html>").arg(m_styleFile).arg(m_cssID).arg(m_text);

    emit doneReading(m_html);
}

