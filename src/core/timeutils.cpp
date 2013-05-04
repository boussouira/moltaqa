#include "timeutils.h"
#include "stringutils.h"

#include <qdatetime.h>
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
    else if(hYear >= aliveDeathYear())
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

QString elapsedTime(uint timestamp)
{
    uint current = QDateTime::currentDateTime().toTime_t();
    uint elapsed = current - timestamp;

    QString str;
    if(elapsed == 0) {
        str = QObject::tr("الان");
    } else if(elapsed < 60) {
        str = Utils::String::Arabic::plural(elapsed, Utils::String::Arabic::SECOND);
    } else if(elapsed < 3600) {
        str = Utils::String::Arabic::plural((elapsed / 60), Utils::String::Arabic::MINUTE);
    } else if(elapsed < 86400) {
        str = Utils::String::Arabic::plural((elapsed / 3600), Utils::String::Arabic::HOUR);
    }/* else if(elapsed < 2592000) {
        str = Utils::String::Arabic::plural((elapsed / 86400), Utils::String::Arabic::DAY);
    } else if(elapsed < 31104000) {
        str = Utils::String::Arabic::plural((elapsed / 2592000), Utils::String::Arabic::MONTH);
    } else if(elapsed < 31104000*30) {
        str = Utils::String::Arabic::plural((elapsed / 31104000), Utils::String::Arabic::YEAR);
    } else {
        str = QString();
    }*/

    if(str.size())
        return str;

    QDateTime date;
    date.setTime_t(timestamp);
    return date.toString("hh:mm - dd/MM");
}

QString prettySeconds(int seconds) {
  seconds = qAbs(seconds);

  int hours = seconds / (60*60);
  int minutes = (seconds / 60) % 60;
  seconds %= 60;

  QString ret;
  if (hours)
    ret.sprintf("%d:%02d:%02d", hours, minutes, seconds);
  else
    ret.sprintf("%d:%02d", minutes, seconds);

  return ret;
}

} // Time
} // Utils
