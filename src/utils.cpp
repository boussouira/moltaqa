#include "utils.h"

#include <qsqlquery.h>
#include <qsqldatabase.h>
#include <qsqlerror.h>
#include <qfileinfo.h>

namespace Utils {

int randInt(int smin, int smax)
{
    return (smin + (qrand() % (smax-smin+1)));
}

QString genBookName(QString path, bool fullPath, QString ext)
{
    QString fileName("book_");
    QString chars("abcdefghijklmnpqrstuvwxyz0123456789");
    int smax = chars.size()-1;

    if(!ext.startsWith('.'))
        ext.prepend('.');

    if(!path.endsWith('/') || !path.endsWith('\\'))
        path.append('/');


    for(int i=0; i<6; i++) {
        fileName.append(chars.at(randInt(0, smax)));
    }

    while(true) {
        if(QFile::exists(path+fileName+ext)){
            fileName.append(chars.at(randInt(0, smax)));
        } else {
            break;
        }
    }

    if(fullPath)
        return path+fileName+ext;
    else
        return fileName+ext;
}

QString arPlural(int count, int word, bool html)
{
    QStringList list;
    QString str;

    if(word == Plural::SECOND)
        list << QObject::tr("ثانية")
             << QObject::tr("ثانيتين")
             << QObject::tr("ثوان")
             << QObject::tr("ثانية");
    else if(word == Plural::MINUTE)
        list << QObject::tr("دقيقة")
             << QObject::tr("دقيقتين")
             << QObject::tr("دقائق")
             << QObject::tr("دقيقة");
    else if(word == Plural::HOUR)
        list << QObject::tr("ساعة")
             << QObject::tr("ساعتين")
             << QObject::tr("ساعات")
             << QObject::tr("ساعة");
    else if(word == Plural::BOOK)
        list << QObject::tr("كتاب واحد")
             << QObject::tr("كتابين")
             << QObject::tr("كتب")
             << QObject::tr("كتابا");
    else if(word == Plural::AUTHOR)
        list << QObject::tr("مؤلف واحد")
             << QObject::tr("مؤلفيين")
             << QObject::tr("مؤلفيين")
             << QObject::tr("مؤلفا");
    else if(word == Plural::CATEGORIE)
        list << QObject::tr("قسم واحد")
             << QObject::tr("قسمين")
             << QObject::tr("أقسام")
             << QObject::tr("قسما");
    else if(word == Plural::FILES)
        list << QObject::tr("ملف واحد")
             << QObject::tr("ملفين")
             << QObject::tr("ملفات")
             << QObject::tr("ملفا");

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

QString secondsToString(int milsec, bool html)
{
    QString time;

    int seconde = (int) ((milsec / 1000) % 60);
    int minutes = (int) (((milsec / 1000) / 60) % 60);
    int hours   = (int) (((milsec / 1000) / 60) / 60);

    if(hours > 0){
        time.append(arPlural(hours, Plural::HOUR, html));
        time.append(QObject::tr(" و "));
    }

    if(minutes > 0 || hours > 0) {
        time.append(arPlural(minutes, Plural::MINUTE, html));
        time.append(QObject::tr(" و "));
    }

    time.append(arPlural(seconde, Plural::SECOND, html));

    return time;
}
}

namespace Log {

void QueryError(QSqlQuery &query, const char *file, int line)
{
    qCritical("[%s:%d] SQL error: %s",
              qPrintable(QFileInfo(file).fileName()), line, qPrintable(query.lastError().text()));
}

void DatabaseError(QSqlDatabase &db, const char *file, int line)
{
    qCritical("[%s:%d] Database error: %s",
              qPrintable(QFileInfo(file).fileName()), line, qPrintable(db.lastError().text()));
}
}
