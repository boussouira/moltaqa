#include "clucenequery.h"
#include "utils.h"
#include <CLucene/analysis/Analyzers.h>
#include <CLucene/search/Query.h>

SearchFilter::SearchFilter() :
    query(0)
{

}

SearchFilter::~SearchFilter()
{
    ml_delete_check(query);
}

CLuceneQuery::CLuceneQuery() :
    searchQuery(0),
    filter(0),
    resultFilter(0),
    searchFieldW(0)
{
}

CLuceneQuery::~CLuceneQuery()
{
    ml_delete_check(searchQuery);
    ml_delete_check(filter);
    ml_delete_check(resultFilter);

    if(searchFieldW)
        free(searchFieldW);
}
