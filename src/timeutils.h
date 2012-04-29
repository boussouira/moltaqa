#ifndef TIMEUTILS_H
#define TIMEUTILS_H

#include <qstring.h>

namespace Utils {

namespace Time {

int hijriToGregorian(int hYear);
int gregorianToHijri(int gYear);

QString hijriYear(int hYear);

QString secondsToString(int milsec, bool html=false);
QString elapsedTime(uint timestamp);

} // Time
} // Utils

#endif // TIMEUTILS_H
