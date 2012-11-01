#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <qregexp.h>

namespace Utils {
namespace Html {

inline QString removeSpecialChars(QString text)
{
    return text.replace(QRegExp("&#?[a-z0-9]{2,8};"), " ");
}

inline QString removeTags(QString text)
{
    return text.contains('<')
            ? text.replace(QRegExp("</?\\w[^>]*>"), " ").trimmed()
            : text;
}

QString jsEscape(QString text);

QString specialCharsEncode(QString text);
QString getTagsText(const QString &text, const QString &tag);

QString nl2br(QString text);
QString format(QString text);
QString removeHTMLFormat(QString text);

} //Html

namespace String {

QString abbreviate(QString str, int size);

namespace Arabic {

enum Words {
    SECOND  = 1,
    MINUTE,
    HOUR,
    DAY,
    MONTH,
    YEAR,
    BOOK,
    AUTHOR,
    CATEGORIE,
    FILES
};

QString plural(int count, Words word, bool html=false);
QString clean(QString text);
inline QString removeTashekil(QString text) { return text.remove(QRegExp("[\\x064B-\\x0653]")); }

bool compare(QChar firstChar, QChar secondChar);

bool compare(QString first, QString second);
bool contains(QString src, QString text);
bool fuzzyContains(QString first, QString second);

QStringList getMatchString(const QString &text, QString searchText);

inline bool isAlef(QChar c) { return (c == 0x0622 || c == 0x0623 || c == 0x0625 || c == 0x0627); }
inline bool isHah(QChar c) { return (c == 0x0629 || c == 0x0647); }
inline bool isYeh(QChar c) { return (c == 0x0649 || c == 0x0649); }
inline bool isTashekil(QChar c) { return (0x064B <= c && c <= 0x0653); }
inline bool isTatweel(QChar c) { return (c == 0x0640); }
inline bool isArabic(QChar c) { return (0x0621 <= c &&  c <= 0x06ED); }

QString arabicNumbers(const QString &text);
inline QString arabicNumbers(int number) { return arabicNumbers(QString::number(number)); }

QString fixShamelaShoorts(QString text);

} //Arabic
} //String
} //Utils

#endif // STRINGUTILS_H
