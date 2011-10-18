#ifndef CLUTILS_H
#define CLUTILS_H

#include <qstring.h>

namespace Utils {

QString WCharToString(const wchar_t *str);
wchar_t* QStringToWChar(const QString &str);

}

#define QSTRING_TO_WCHAR(s) (const wchar_t*)s.utf16()

#endif // CLUTILS_H
