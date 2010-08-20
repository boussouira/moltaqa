#include "textformatter.h"

TextFormatter::TextFormatter()
{
    laodSettings();
}

void TextFormatter::laodSettings()
{
    QSettings settings;
    QString style = settings.value("General/style", "Default").toString();
    m_styleFile = settings.value(QString("%1_style/style_file_path").arg(style)).toString();
#ifdef Q_OS_WIN32
    m_styleFile.prepend("file:///");
#else
    m_styleFile.prepend("file://");
#endif
}

QString TextFormatter::getText()
{
    return QString("<html>"
                   "<head>"
                   "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />"
                   "<link href= \"%1\" rel=\"stylesheet\" type=\"text/css\"/>"
                   "</head>"
                   "<body>"
                   "<div id=\"%2\">%3</div>"
                   "</body></html>").arg(m_styleFile).arg(m_cssID).arg(m_text);
}

void TextFormatter::insertText(QString text)
{
    text.replace(QRegExp("[\\r\\n]"), "<br/>");
    m_text.append(text);
}

void TextFormatter::clearText()
{
    m_text.clear();
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
