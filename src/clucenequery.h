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

    enum SearchSort {
        Relvance,
        BookRelvance,
        BookPage,
        DeathRelvance,
        DeathBookPage
    };

    lucene::search::Query* searchQuery;
    lucene::search::Query *filterQuery;
    lucene::search::BooleanClause::Occur filterClause;
    lucene::search::Query *resultFilterQuery;
    lucene::search::BooleanClause::Occur resultFilterClause;
    QString searchField;
    wchar_t *searchFieldW;
    SearchSort sort;
};

#endif // CLUCENEQUERY_H
