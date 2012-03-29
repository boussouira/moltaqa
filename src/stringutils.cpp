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

QString formatHTML(QString text, bool useBrTag)
{
    QString htmlText;
    text = Utils::htmlSpecialCharsEncode(text);

    QStringList paragraphs = text.split(QRegExp("[\\r\\n]+"));
    if(useBrTag) {
        foreach(QString p, paragraphs) {
            htmlText.append(p.simplified());
            htmlText.append("<br />");
        }
    } else {
        foreach(QString p, paragraphs) {
            htmlText.append("<p>");
            htmlText.append(p.simplified());
            htmlText.append("</p>");
        }
    }

    return htmlText;
}

}
