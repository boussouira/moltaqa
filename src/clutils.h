#ifndef CLUTILS_H
#define CLUTILS_H

#include <qstring.h>

namespace lucene {
    namespace search {
        class Query;
    }
}

namespace Utils {

QString WCharToString(const wchar_t *str);
wchar_t* QStringToWChar(const QString &str);
wchar_t* intToWChar(int num, int radix=10);
wchar_t* intToWChar(int num, wchar_t *dest, int radix);
int WCharToInt(const wchar_t *str);
QString highlightText(QString orignalText, lucene::search::Query *query, bool fragment);
}

#ifndef Q_OS_WIN
    #define _itow _itot
    #define _wtoi _ttoi
#endif

#endif // CLUTILS_H
