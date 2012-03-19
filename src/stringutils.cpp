#include "stringutils.h"

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
}
