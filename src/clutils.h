#ifndef CLUTILS_H
#define CLUTILS_H

#include <qstring.h>
#include <CLucene/StdHeader.h>
#include <CLucene/config/repl_wchar.h>

namespace lucene {
    namespace search {
        class Query;
    }
}

class CLuceneQuery;

#ifndef Q_OS_WIN
    #define _itow _itot
    #define _wtoi _ttoi
#endif
namespace Utils {
namespace CLucene {

wchar_t* QStringToWChar(const QString &str);
wchar_t* intToWChar(int num, int radix=10);
QString highlightText(QString orignalText, lucene::search::Query *query,
                      const wchar_t* field, bool fragment);

QString highlightText(QString orignalText, CLuceneQuery *query, bool fragment);

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
}

#endif // CLUTILS_H
