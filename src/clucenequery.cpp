#include "clucenequery.h"
#include "utils.h"
#include <CLucene/analysis/Analyzers.h>
#include <CLucene/search/Query.h>

CLuceneQuery::CLuceneQuery() :
    searchQuery(0),
    filterQuery(0),
    resultFilterQuery(0),
    searchFieldW(0)
{
}

CLuceneQuery::~CLuceneQuery()
{
    ml_delete_check(searchQuery);
    ml_delete_check(filterQuery);
    ml_delete_check(resultFilterQuery);

    if(searchFieldW)
        free(searchFieldW);
}
