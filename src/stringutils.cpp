#include "stringutils.h"
#include <qstringlist.h>

namespace Utils {

namespace Html {

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
    if(!text.contains(tag))
        return QString();

    QString result;
    QRegExp rx(QString("<%1>(.+)</%1>").arg(tag));
    rx.setMinimal(true);
    int pos = 0;

    while ((pos = rx.indexIn(text, pos)) != -1) {
        result.append(removeTags(rx.cap(1)));
        result.append(" ");
        pos += rx.matchedLength();
    }

    return result;
}

QString format(QString text)
{
    QString htmlText;
    text = specialCharsEncode(text);

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

    if(count <= 1)
        str = list.at(0);
    else if(count == 2)
        str = list.at(1);
    else if (count > 2 && count <= 10)
        str = QString("%1 %2").arg(count).arg(list.at(2));
    else if (count > 10)
        str = QString("%1 %2").arg(count).arg(list.at(3));
    else
        str = QString();

    return html ? QString("<strong>%1</strong>").arg(str) : str;
}

QString clean(QString text)
{
    text = text.trimmed();
    text.replace(QRegExp("[\\x0622\\x0623\\x0625]"), QChar(0x0627));//ALEFs
    text.replace(QChar(0x0629), QChar(0x0647)); //TAH_MARBUTA -> HEH
    text.replace(QChar(0x0649), QChar(0x064A)); //YAH -> ALEF MAKSOURA
    text.remove(QRegExp("[\\x064B-\\x0653]"));

    return text;
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

} // Arabic
} // String
} // Utils
