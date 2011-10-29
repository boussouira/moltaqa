#ifndef LIBRARYSEARCHER_H
#define LIBRARYSEARCHER_H

#include <qthread.h>
#include <qsqldatabase.h>
#include <qhash.h>
#include "clheader.h"
#include "libraryinfo.h"
#include "librarymanager.h"
#include "searchresult.h"

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

    void setQuery(Query *searchQuery, Query *filterQuery, BooleanClause::Occur filterClause);

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
    Query* m_searchQuery;
    Query* m_filterQuery;
    Query* m_query;
    BooleanClause::Occur m_filterClause;
    QString m_queryStr;
    int m_currentPage;
    int m_pageCount;
    int m_timeSearch;
    int m_resultParPage;
    QHash<int, SearchResult*> m_resultsHash;
};

#endif // LIBRARYSEARCHER_H
