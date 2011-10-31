#ifndef CLUTILS_H
#define CLUTILS_H

#include <qstring.h>

namespace lucene {
    namespace search {
        class Query;
    }
}

#ifndef Q_OS_WIN
    #define _itow _itot
    #define _wtoi _ttoi
#endif

namespace Utils {

wchar_t* QStringToWChar(const QString &str);
wchar_t* intToWChar(int num, int radix=10);
QString highlightText(QString orignalText, lucene::search::Query *query, bool fragment);


inline wchar_t* intToWChar(int num, wchar_t *dest, int radix)
{
    return _itow(num, dest, radix);
}

inline QString WCharToString(const wchar_t *str)
{
    return QString::fromWCharArray(str);
}

inline int WCharToInt(const wchar_t *str)
{
    return _wtoi(str);
}

}

#endif // CLUTILS_H
