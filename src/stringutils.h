#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <qregexp.h>

namespace Utils {

inline QString removeHtmlSpecialChars(QString text)
{
    return text.replace(QRegExp("&#?[a-z0-9]{2,8};"), " ");
}

inline QString removeHtmlTags(QString text)
{
    return text.contains('<')
            ? text.replace(QRegExp("</?\\w[^>]*>"), " ").trimmed()
            : text;
}

QString htmlSpecialCharsEncode(QString text);
QString getTagsText(const QString &text, const QString &tag);

QString formatHTML(QString text, bool useBrTag=true);
}

#endif // STRINGUTILS_H
