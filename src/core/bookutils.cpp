#include "bookutils.h"
#include <qregexp.h>
#include <qsqldatabase.h>
#include <qsqlrecord.h>

#define _u(x) QString::fromUtf8(x)

namespace Utils {
namespace Book {

int getPageTitleID(QList<int> &titles, int pageID)
{
    if(!titles.contains(pageID)) {
        int title = 0;
        for(int i=0; i<titles.size(); i++) {
            title = titles.at(i);
            if(i == titles.size()-1)
                return titles.last();
            else if(title <= pageID && pageID < titles.at(i+1))
                return title;
            if(title > pageID)
                break;
        }

        if(titles.size())
            return titles.first();
    }

    return pageID;
}

bool hasShamelaShoorts(const QString &text)
{
    for(int i=0; i<text.size(); i++) {
        ushort ch = text[i].unicode();
        if(0x41 <= ch && ch <= 0x45)
            return true;
    }

    return false;
}

QString fixShamelaShoorts(QString text)
{
    text.replace(QRegExp(_u("([\\x0621-\\x06ED]\\W)A\\b")), _u("\\1""صلى الله عليه وسلم"));
    text.replace(QRegExp(_u("([\\x0621-\\x06ED]\\W)B([هماـ]*)\\b")), _u("\\1""رضي الله عن""\\2"));
    text.replace(QRegExp(_u("([\\x0621-\\x06ED]\\W)C\\b")), _u("\\1""رحمه الله"));
    text.replace(QRegExp(_u("([\\x0621-\\x06ED]\\W)D\\b")), _u("\\1""عز وجل"));
    text.replace(QRegExp(_u("([\\x0621-\\x06ED]\\W)E\\b")), _u("\\1""عليه الصلاة و السلام"));

    return text;
}

QString shamelaQueryFields(const QSqlDatabase &db, QString tableName)
{
    QSqlRecord rec = db.record(tableName);

    QString queryFields = "id, page, part";

    if(rec.indexOf("aya") != -1 && rec.indexOf("sora") != -1)
        queryFields.append(", aya, sora");

    if(rec.indexOf("hno") != -1)
        queryFields.append(", hno");

    return queryFields;
}

}
}
