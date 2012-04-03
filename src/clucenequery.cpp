#include "clucenequery.h"
#include "utils.h"
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
    ML_DELETE_CHECK(searchQuery);
    ML_DELETE_CHECK(filterQuery);

    if(searchFieldW)
        free(searchFieldW);
}