#ifndef CLUCENEQUERY_H
#define CLUCENEQUERY_H

#include <CLucene/StdHeader.h>
#include <CLucene/search/BooleanClause.h>
#include <qstring.h>

namespace lucene {
    namespace search {
        class Query;
    }
}

class SearchFilter {
public:
    SearchFilter();
    ~SearchFilter();
    lucene::search::Query *query;
    lucene::search::BooleanClause::Occur clause;
    int selected;
    int unSelected;
};

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


    lucene::search::Query* searchQuery; /// The search query, must be set before passing to LibrarySearcher
    SearchFilter *filter;               /// Search filter, search without filter if null
    SearchFilter *resultFilter;         /// Filter that may be set from ResultWidget, search without filter if null
    QString searchField;                /// Default search filed
    wchar_t *searchFieldW;              /// Default search filed
    SearchSort sort;                    /// Result sorting
};

#endif // CLUCENEQUERY_H
