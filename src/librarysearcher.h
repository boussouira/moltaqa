#ifndef LIBRARYSEARCHER_H
#define LIBRARYSEARCHER_H

#include <qcache.h>
#include <qthread.h>

class LibraryInfo;
class LibraryManager;
class SearchResult;
class SearchResultReader;
class CLuceneQuery;
namespace lucene {
namespace search {
class Hits;
class IndexSearcher;
class Query;
class Sort;
class SortField;
}
}

class LibrarySearcher : public QThread
{
    Q_OBJECT

public:
    enum Action {
        SEARCH,
        FETECH
    };

    LibrarySearcher(QObject *parent = 0);
    ~LibrarySearcher();

    void run();
    void stop();

    void setQuery(CLuceneQuery *query);

    SearchResult *getResult(int resultD);
    CLuceneQuery *getSearchQuery();
    SearchResultReader *getResultReader();

    int pageCount();
    int currentPage();
    int resultsCount();
    int searchTime();
    int resultsPeerPage();

    int fetchStart();
    int fetchEnd();
    QList<int> resultsToFetch();

    void nextPage();
    void prevPage();
    void firstPage();
    void lastPage();
    void page(int page);

protected:
    void open();
    void buildQuery();
    void search();
    void fetech();
    void fetechResult(int rid);

signals:
    void startSearching();
    void doneSearching();
    void startFeteching();
    void doneFeteching();
    void gotResult(SearchResult *result);
    void gotException(QString what, int id);

protected:
    friend class ResultReader;

protected:
    LibraryInfo *m_libraryInfo;
    LibraryManager *m_libraryManager;
    Action m_action;
    lucene::search::IndexSearcher *m_searcher;
    lucene::search::Hits* m_hits;
    lucene::search::Query* m_query;
    lucene::search::Sort *m_sort;
    CLuceneQuery *m_cluceneQuery;
    SearchResultReader *m_resultReader;
    int m_currentPage;
    int m_pageCount;
    int m_timeSearch;
    int m_resultParPage;
    QCache<int, SearchResult> m_resultsHash;
    bool m_stop;
};

#endif // LIBRARYSEARCHER_H
