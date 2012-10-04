#ifndef TIMEUTILS_H
#define TIMEUTILS_H

#include <qstring.h>

namespace Utils {

namespace Time {

inline int maxDeathYear() { return 1433; }
inline int unknowDeathYear() { return maxDeathYear() + 200; }
inline int aliveDeathYear() { return maxDeathYear() + 100; }

int hijriToGregorian(int hYear);
int gregorianToHijri(int gYear);

QString hijriYear(int hYear);

QString secondsToString(int milsec, bool html=false);
QString elapsedTime(uint timestamp);

QString prettySeconds(int seconds);
inline QString prettyMilliSeconds(int msec)  { return prettySeconds(msec/1000); }

} // Time
} // Utils

#endif // TIMEUTILS_H
