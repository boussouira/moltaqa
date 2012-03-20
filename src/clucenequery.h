#ifndef CLUCENEQUERY_H
#define CLUCENEQUERY_H

#include <qstring.h>
#include <CLucene/StdHeader.h>
#include <CLucene/search/BooleanClause.h>

namespace lucene {
    namespace search {
        class Query;
    }
}

class CLuceneQuery
{
public:
    CLuceneQuery();
    ~CLuceneQuery();

    lucene::search::Query* searchQuery;
    lucene::search::Query *filterQuery;
    lucene::search::BooleanClause::Occur filterClause;
    QString searchField;
    wchar_t *searchFieldW;
};

#endif // CLUCENEQUERY_H
