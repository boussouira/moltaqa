#include "clucenequery.h"
#include <CLucene/analysis/Analyzers.h>
#include <CLucene/search/Query.h>

CLuceneQuery::CLuceneQuery() :
    searchQuery(0),
    filterQuery(0),
    searchFieldW(0)
{
}

CLuceneQuery::~CLuceneQuery()
{
    if(searchQuery)
        delete searchQuery;

    if(filterQuery)
        delete filterQuery;

    if(searchFieldW)
        free(searchFieldW);
}
