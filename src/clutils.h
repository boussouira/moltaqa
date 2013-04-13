#ifndef CLUTILS_H
#define CLUTILS_H

#include <CLucene/StdHeader.h>
#include <CLucene/config/repl_wchar.h>
#include <qstring.h>

namespace lucene {
    namespace search {
        class Query;
    }

    namespace queryParser {
        class QueryParser;
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
QString clearSpecialChars(const QString &text);

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

lucene::search::Query *parse(lucene::queryParser::QueryParser *queryPareser,
                             const QString &text, bool andOperator);

lucene::search::Query *termQuery(const QString &queryStr, const wchar_t *field);
}
}

#endif // CLUTILS_H
