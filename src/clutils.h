#ifndef CLUTILS_H
#define CLUTILS_H

#include <qstring.h>

namespace Utils {

QString WCharToString(const wchar_t *str);
wchar_t* QStringToWChar(const QString &str);

}

#ifdef Q_OS_WIN
    #define QSTRING_TO_WCHAR(s) (const wchar_t*)s.utf16()
#else
    #define QSTRING_TO_WCHAR(s) Utils::QStringToWChar(s)
    #define _itow _itot
#endif

#endif // CLUTILS_H
