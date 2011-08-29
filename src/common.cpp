#include "common.h"

int rand_int(int smin, int smax)
{
    return (smin + (qrand() % (smax-smin+1)));
}

QString genBookName(QString path, bool fullPath)
{
    QString fileName("book_");
    char c[] = "abcdefABCDEF1234567";
    int len = strlen(c);

    for(int i=0; i<6; i++) {
        fileName.append(c[rand_int(0, len-1)]);
    }

    while(true) {
        if(QFile::exists(path+"/"+fileName+".alb")){
            fileName.append(c[rand_int(0, len-1)]);
        } else {
            break;
        }
    }

    if(fullPath)
        return path+"/"+fileName+".alb";
    else
        return fileName+".alb";
}

QString arPlural(int count, PULRAL word, bool html)
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
        time.append(arPlural(hours, HOUR, html));
        time.append(QObject::tr(" و "));
    }

    if(minutes > 0 || hours > 0) {
        time.append(arPlural(minutes, MINUTE, html));
        time.append(QObject::tr(" و "));
    }

    time.append(arPlural(seconde, SECOND, html));

    return time;
}
