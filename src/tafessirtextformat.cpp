#include "tafessirtextformat.h"
#include <qdebug.h>

TafessirTextFormat::TafessirTextFormat()
{
}
void TafessirTextFormat::appendTafessirText(QString pTafessirText)
{
    QString tafessirText = pTafessirText.split(QRegExp("\\[co\\]")).first();
    QString footnoteText = pTafessirText.split(QRegExp("\\[co\\]")).last();

    tafessirText.replace(QRegExp("\\(([0-9]{1,2})\\)"), "<a class=\"footn\" id=\"fnb\\1\" href=\"#fn\\1\">(\\1)</a>");
    tafessirText.replace("[br]","<br/>");

    footnoteText.replace("[br]","<br/>");
    footnoteText.prepend("<br>______<br><footnote>");
    footnoteText.replace("[/co]","</footnote>");
    footnoteText.replace(QRegExp("\\(([0-9]{1,2})\\)"), "<a href=\"#fnb\\1\" id=\"fn\\1\">(\\1)</a>");
    m_text.append(tafessirText);
    m_text.append(footnoteText);
}

QString TafessirTextFormat::getTafessirText()
{
    return QString("<html>"
                   "<head>"
                   "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />"
                   "<link href= \"qrc:/css/defaut.css\"  rel=\"stylesheet\" type=\"text/css\"/>"
                   "</head>"
                   "<body>"
                   "<div id=\"tafessir\">%1</div>"
                   "</div></body></html>").arg(m_text);
}

void TafessirTextFormat::clearTafessirText()
{
    m_text.clear();
}
