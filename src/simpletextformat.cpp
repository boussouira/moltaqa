#include "simpletextformat.h"

SimpleTextFormat::SimpleTextFormat()
{
}

QString SimpleTextFormat::formatText()
{
    QString txt = m_cleanText;
    txt.replace(QRegExp("[\\r\\n]"), "<br/>");
    return QString("<html>"
                   "<head>"
                   "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />"
                   "<link href= \"%1\"  rel=\"stylesheet\" type=\"text/css\"/>"
                   "</head>"
                   "<body>"
                   "<div id=\"simpleBook\">%2</div>"
                   "</div></body></html>").arg(m_styleFile).arg(txt);
}
