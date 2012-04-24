#ifndef LIBRARYSEARCHER_H
#define LIBRARYSEARCHER_H

#include <qthread.h>
#include <qsqldatabase.h>
#include <qhash.h>
#include "clheader.h"
#include "libraryinfo.h"
#include "librarymanager.h"
#include "searchresult.h"
#include "clucenequery.h"

class SearchResultReader;

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
    int pageCount();
    int currentPage();
    int resultsCount();
    int searchTime();
    int resultsPeerPage();

    void nextPage();
    void prevPage();
    void firstPage();
    void lastPage();
    void fetechResults(int page);

protected:
    void open();
    void buildQuery();
    void search();
    void fetech();

signals:
    void startSearching();
    void doneSearching();
    void startFeteching();
    void doneFeteching();
    void gotResult(SearchResult *result);
    void gotException(QString what, int id);

protected:
    LibraryInfo *m_libraryInfo;
    LibraryManager *m_libraryManager;
    Action m_action;
    IndexSearcher *m_searcher;
    Hits* m_hits;
    Query* m_query;
    Sort *m_sort;
    CLuceneQuery *m_cluceneQuery;
    SearchResultReader *m_resultReader;
    int m_currentPage;
    int m_pageCount;
    int m_timeSearch;
    int m_resultParPage;
    QHash<int, SearchResult*> m_resultsHash;
    bool m_stop;
};

#endif // LIBRARYSEARCHER_H
