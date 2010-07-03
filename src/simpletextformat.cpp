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
                   "<link href= \"qrc:/css/defaut.css\"  rel=\"stylesheet\" type=\"text/css\"/>"
                   "</head>"
                   "<body>"
                   "<div id=\"simpleBook\">%1</div>"
                   "</div></body></html>").arg(txt);
}
