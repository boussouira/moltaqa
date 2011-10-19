#ifndef CLUTILS_H
#define CLUTILS_H

#include <qstring.h>

namespace Utils {

QString WCharToString(const wchar_t *str);
wchar_t* QStringToWChar(const QString &str);

}

#ifndef Q_OS_WIN
    #define _itow _itot
#endif

#endif // CLUTILS_H
