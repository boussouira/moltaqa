#include "stringutils.h"
#include <qstringlist.h>

namespace Utils {

QString htmlSpecialCharsEncode(QString text)
{
     text.replace('&', "&amp;");
     text.replace('\'', "&#39;");
     text.replace('\"', "&quot;");
     text.replace('<', "&lt;");
     text.replace('>', "&gt;");

     return text;
}

QString getTagsText(const QString &text, const QString &tag)
{
    if(!text.contains(tag))
        return QString();

    QString result;
    QRegExp rx(QString("<%1>(.+)</%1>").arg(tag));
    rx.setMinimal(true);
    int pos = 0;

    while ((pos = rx.indexIn(text, pos)) != -1) {
        result.append(removeHtmlTags(rx.cap(1)));
        result.append(" ");
        pos += rx.matchedLength();
    }

    return result;
}

QString formatHTML(QString text)
{
    QString htmlText;
    text = Utils::htmlSpecialCharsEncode(text);

    QStringList paragraphs = text.split(QRegExp("[\\r\\n]{2,}"), QString::SkipEmptyParts);
    foreach(QString p, paragraphs) {
        QStringList lines = p.split(QRegExp("[\\r\\n]"), QString::SkipEmptyParts);

        htmlText.append("<p>");

        for(int i=0;i<lines.size(); i++) {
            if(i)
                htmlText.append("<br />");

            htmlText.append(lines[i].simplified());
        }

        htmlText.append("</p>");
    }

    return htmlText;
}

}
