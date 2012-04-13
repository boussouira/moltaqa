#include "timeutils.h"
#include "stringutils.h"
#include <qobject.h>

namespace Utils {

namespace Time {

int hijriToGregorian(int hYear)
{
    return (hYear + 622) - (hYear / 33);
}

int gregorianToHijri(int gYear)
{
    return  (gYear - 622) + ((gYear - 622) / 32);
}

QString hijriYear(int hYear)
{
    if(hYear <= 0)
        return QObject::tr("%1 م").arg(Utils::Time::hijriToGregorian(hYear));
    else if(hYear >= 99999)
        return QObject::tr("معاصر");
    else
        return QObject::tr("%1 هـ").arg(hYear);
}

QString secondsToString(int milsec, bool html)
{
    QString time;

    int seconde = (int) ((milsec / 1000) % 60);
    int minutes = (int) (((milsec / 1000) / 60) % 60);
    int hours   = (int) (((milsec / 1000) / 60) / 60);

    if(hours > 0){
        time.append(String::Arabic::plural(hours, String::Arabic::HOUR, html));
        time.append(QObject::tr(" و "));
    }

    if(minutes > 0 || hours > 0) {
        time.append(String::Arabic::plural(minutes, String::Arabic::MINUTE, html));
        time.append(QObject::tr(" و "));
    }

    time.append(String::Arabic::plural(seconde, String::Arabic::SECOND, html));

    return time;
}

} // Time
} // Utils
