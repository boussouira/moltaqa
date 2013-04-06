#include "stringutils.h"
#include "clconstants.h"
#include <qstringlist.h>
#include <qsharedpointer.h>

#define _u QString::fromUtf8

namespace Utils {

namespace Html {

QString removeSpecialChars(QString text)
{
    return text.contains('&')
            ? text.replace(QRegExp("&#?[a-z0-9]{2,8};"), " ")
            : text;
}

QString removeTags(const QString &text)
{
    // Check if we have any tag
    if(text.indexOf('<') == -1)
        return text;

    QString buf;
    bool insideTag = false;
    QChar lt('<');
    QChar gt('>');
    QChar sp(' ');

    buf.reserve(text.size());
    for (int i=0; i<text.size(); i++) {
        const QChar ch = text[i];
        if (ch==lt) {
            if(i+1<text.size()
                    && (IS_ASCII(text[i+1]) || text[i+1] == '/')) {
                insideTag = true;
            } else {
                buf.append(ch);
            }
        } else if (ch==gt) {
            if(buf.size()) {
                if(insideTag)
                    buf.append(sp);
                else
                    buf.append(ch);
            }

            insideTag = false;

        } else if (!insideTag) {
            buf.append(ch);
        }
    }

    return buf;
}

QString jsEscape(QString text)
{
    return text.replace('\\', "\\\\")
            .replace('"', "\\\"")
            .replace('\'', "\\'")
            .replace('\t', "\\t")
            .replace('\n', "\\n")
            .replace('\r', "\\r");
}

QString specialCharsEncode(QString text)
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
    QString buf;
    QString startTag = "<" + tag;
    QString endTag = "</" + tag + ">";

    int initOffset = text.indexOf(startTag, 0, Qt::CaseInsensitive);
    if(initOffset == -1)
        return QString();

    buf.reserve(text.size());

    for(int i=initOffset; i<text.size(); i++) {
        if(text[i] == startTag[0]) { // find the tag start
            int j = 1;
            bool match = false;
            int matcheLen = 1;
            for(int k=i+1; k<text.size() && j <startTag.size(); ++j, ++k) {

                ++matcheLen;

                if (startTag[j].toLower() == text[k].toLower()) {
                    match = true;
                } else {
                    match = false;
                    break;
                }
            }

            if(match) { // find > in starting tag
                int t=0;
                QChar gt('>');
                for(int k=i+matcheLen; i<text.size();k++) {
                    ++t;
                    if(text[k]==gt) {
                        int n=i+matcheLen+t;
                        int closeIndex = text.indexOf(endTag, n, Qt::CaseInsensitive);
                        if(closeIndex != -1) {
                            buf.append(removeTags(text.mid(n, closeIndex-n)));
                            buf.append(' ');

                            i = closeIndex + endTag.size();
                        }

                        break;
                    }
                }
            }
        }
    }

    return buf;
}

QString nl2br(QString text)
{
    QString htmlText;
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

QString format(QString text)
{
    QString htmlText = text;
    htmlText = specialCharsEncode(htmlText);
    htmlText = nl2br(htmlText);

    return htmlText;
}

QString removeHTMLFormat(QString text)
{
    text = text.replace(QRegExp("(</p>|<br ?/?>)"), "\n");
    return removeTags(text);
}
} //Html

namespace String {

QString abbreviate(QString str, int size)
{
    if (str.length() <= size-3)
            return str;

    str.simplified();
    int index = str.lastIndexOf(' ', size-3);
    if (index <= -1)
            return "";

    return str.left(index).append("...");
}

namespace Arabic {

QString plural(int count, Words word, bool html)
{
    QStringList list;
    QString str;

    if(word == SECOND)
        list << QObject::tr("ثانية")
             << QObject::tr("ثانيتين")
             << QObject::tr("ثوان")
             << QObject::tr("ثانية");
    else if(word == MINUTE)
        list << QObject::tr("دقيقة")
             << QObject::tr("دقيقتين")
             << QObject::tr("دقائق")
             << QObject::tr("دقيقة");
    else if(word == HOUR)
        list << QObject::tr("ساعة")
             << QObject::tr("ساعتين")
             << QObject::tr("ساعات")
             << QObject::tr("ساعة");
    else if(word == DAY)
        list << QObject::tr("يوم")
             << QObject::tr("يومين")
             << QObject::tr("ايام")
             << QObject::tr("يوما");
    else if(word == MONTH)
        list << QObject::tr("شهر")
             << QObject::tr("شهرين")
             << QObject::tr("اشهر")
             << QObject::tr("شهرا");
    else if(word == YEAR)
        list << QObject::tr("عام")
             << QObject::tr("عامين")
             << QObject::tr("اعوام")
             << QObject::tr("عاما");
    else if(word == BOOK)
        list << QObject::tr("كتاب واحد")
             << QObject::tr("كتابين")
             << QObject::tr("كتب")
             << QObject::tr("كتابا");
    else if(word == AUTHOR)
        list << QObject::tr("مؤلف واحد")
             << QObject::tr("مؤلفيين")
             << QObject::tr("مؤلفيين")
             << QObject::tr("مؤلفا");
    else if(word == CATEGORIE)
        list << QObject::tr("قسم واحد")
             << QObject::tr("قسمين")
             << QObject::tr("أقسام")
             << QObject::tr("قسما");
    else if(word == FILES)
        list << QObject::tr("ملف واحد")
             << QObject::tr("ملفين")
             << QObject::tr("ملفات")
             << QObject::tr("ملفا");
    else
        qWarning("Utils::Arabic::plural word %d is not handled", word);

    if(count == 1)
        str = list.at(0);
    else if(count == 2)
        str = list.at(1);
    else if (count > 2 && count <= 10)
        str = QString("%1 %2").arg(count).arg(list.at(2));
    else if (count > 10)
        str = QString("%1 %2").arg(count).arg(list.at(3));
    else
        str = QLatin1String("0");

    return html ? QString("<strong>%1</strong>").arg(str) : str;
}

QString clean(QString text)
{
    QString buf;
    buf.reserve(text.size());

    for(int i=0; i<text.size(); i++) {
        QChar ch = text[i];

        switch(ch.unicode()) {
        case 0x0622: // ALEF WITH MADDA ABOVE
        case 0x0623: // ALEF WITH HAMEZA ABOVE
        case 0x0625: // ALEF WITH HAMEZA BELOW
            buf.append(QChar(0x0627)); // ALEF
            break;
        case 0x0629: // TAH MARBUTA
             buf.append(QChar(0x0647)); // HAH
            break;
        case 0x0649: // ALEF MAKSURA
             buf.append(QChar(0x064A)); // YEH
            break;
        case 0x064b:
        case 0x064c:
        case 0x064d:
        case 0x064e:
        case 0x064f:
        case 0x0650:
        case 0x0651:
        case 0x0652:
        case 0x0653:
            break;
        default:
            buf.append(ch);
        }
    }

    return buf;
}

bool compare(QChar firstChar, QChar secondChar) {
    return ((firstChar == secondChar)
            || (isAlef(firstChar) && isAlef(secondChar))
            || (isHah(firstChar) && isHah(secondChar))
            || (isYeh(firstChar) && isYeh(secondChar)));
}

bool compare(QString first, QString second)
{
    return clean(first) == clean(second);
}

bool contains(QString src, QString text)
{
    QString cleanSrc = clean(src);
    QString cleanText = clean(text);

    return cleanSrc.contains(cleanText);
}

bool fuzzyContains(QString first, QString second)
{
    QString cleanFirst= clean(first);
    QString cleanSecond = clean(second);

    if(cleanFirst.size() < cleanSecond.size())
        return cleanSecond.contains(cleanFirst);
    else
        return cleanFirst.contains(cleanSecond);
}

class ArabicFindMatch {
public:
    ArabicFindMatch() : offset(0), length(0) {}
    ArabicFindMatch(int _offset, int _length) : offset(_offset), length(_length) {}

    typedef QSharedPointer<ArabicFindMatch> Ptr;

    int offset;
    int length;
};

ArabicFindMatch::Ptr arabicFind(const QString &text, const QString & pattern, int pos) {
    if (pos < 0) {
        pos = 0;
    }

    QString lower = pattern.toLower();
    int patternLength = lower.size();
    int last = text.size();
    QChar firstChar = lower[0];

    for (int i = pos; i < last; i++) {
        if (compare(text[i], firstChar)) {
            int j = 1;
            bool match = false;
            int matcheLen = 1;
            for (int k = i + 1; j < patternLength && k < text.size() && j < lower.size(); ++j, ++k) {
                if(isTashekil(lower[j])) { --k; continue; }
                if(isTashekil(text[k])) { --j; ++matcheLen; continue; }

                ++matcheLen;

                if (compare(lower[j], text[k])) {
                    match = true;
                } else {
                    match = false;
                    break;
                }
            }

            while(match && i+matcheLen < text.size() && isTashekil(text[i+matcheLen]))
                ++matcheLen;

            if (match) {
                return ArabicFindMatch::Ptr(new ArabicFindMatch(i, matcheLen));
            }
        }
    }

    return ArabicFindMatch::Ptr();
}

QStringList getMatchString(const QString &text, QString searchText)
{
    QStringList matches;

    for(int i=0; i<text.size();i++) {
        ArabicFindMatch::Ptr match = arabicFind(text, searchText, i);
        if(match) {
            matches.append(text.mid(match->offset, match->length));
            i = match->offset+match->length-1;
        }
    }

    return matches;
}

QString arabicNumbers(const QString &text)
{
    QString arabicNum;
    for(int i=0; i<text.size(); i++) {
        QChar c = text.at(i);
        ushort uc = c.unicode();

        if(0x30 <= uc && uc <= 0x39) {
            arabicNum.append(QChar(0x630 + uc));
        } else {
            arabicNum.append(c);
        }
    }

    return arabicNum;
}

} // Arabic
} // String
} // Utils
