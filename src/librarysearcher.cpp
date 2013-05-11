#include "librarysearcher.h"
#include "abstractbookreader.h"
#include "clconstants.h"
#include "clheader.h"
#include "clucenequery.h"
#include "clutils.h"
#include "librarybook.h"
#include "librarybookmanager.h"
#include "libraryinfo.h"
#include "librarymanager.h"
#include "mainwindow.h"
#include "searchresult.h"
#include "searchresultreader.h"
#include "stringutils.h"
#include "utils.h"

#include <qdatetime.h>
#include <qfuture.h>
#include <qtconcurrentmap.h>

LibrarySearcher::LibrarySearcher(QObject *parent)
    : QThread(parent),
      m_action(SEARCH),
      m_searcher(0),
      m_hits(0),
      m_query(0),
      m_cluceneQuery(0),
      m_stop(false)
{
    m_libraryInfo = MW->libraryInfo();
    m_libraryManager = LibraryManager::instance();
    m_resultReader = new SearchResultReader(this);
    m_sort = new Sort();

    m_resultParPage = Utils::Settings::get("Search/resultPeerPage", 10).toInt();

    m_resultsHash.setMaxCost(m_resultParPage * 3);
}

LibrarySearcher::~LibrarySearcher()
{
    ml_delete_check(m_hits);
    ml_delete_check(m_query);
    ml_delete_check(m_sort);
    ml_delete_check(m_cluceneQuery);
    ml_delete_check(m_resultReader);

    if(m_searcher) {
        m_searcher->close();
        delete m_searcher;
    }
}

void LibrarySearcher::run()
{
    try {
        if(m_action == SEARCH){
            search();

            if(m_hits->length() > 0)
                fetech();

        } else if (m_action == FETECH) {
            fetech();
        }
    } catch(CLuceneError &e) {
        qCritical("LibrarySearcher::run CLucene exception %s", e.what());
        emit gotException(e.what(), e.number());
    } catch(std::exception &e){
        qCritical("LibrarySearcher::run STD exception %s", e.what());
        emit gotException(e.what(), 0);
    } catch(...) {
        qCritical() << "LibrarySearcher::run Unknow exception when searching at"
                    << m_libraryInfo->indexDataDir();

        emit gotException("UNKNOW", -1);
    }
}

void LibrarySearcher::stop()
{
    m_stop = true;
}

void LibrarySearcher::open()
{
    if(!m_searcher) {
        m_searcher = new IndexSearcher(qPrintable(m_libraryInfo->indexDataDir()));
    }
}

void LibrarySearcher::buildQuery()
{
    BooleanQuery *booleanQuery = new BooleanQuery();
    booleanQuery->add(m_cluceneQuery->searchQuery, BooleanClause::MUST);

    if(m_cluceneQuery->filter)
        booleanQuery->add(m_cluceneQuery->filter->query, m_cluceneQuery->filter->clause);

    if(m_cluceneQuery->resultFilter)
        booleanQuery->add(m_cluceneQuery->resultFilter->query, m_cluceneQuery->resultFilter->clause);

    m_query = m_searcher->rewrite(booleanQuery);

    if(m_cluceneQuery->sort == CLuceneQuery::BookRelvance) {
        SortField *sort[] = {new SortField(BOOK_ID_FIELD), SortField::FIELD_SCORE(), NULL};
        m_sort->setSort(sort);
    } else if (m_cluceneQuery->sort == CLuceneQuery::BookPage) {
        SortField *sort[] = {new SortField(BOOK_ID_FIELD), new SortField(PAGE_ID_FIELD), NULL};
        m_sort->setSort(sort);
    } else if (m_cluceneQuery->sort == CLuceneQuery::DeathRelvance) {
        SortField *sort[] = {new SortField(AUTHOR_DEATH_FIELD), SortField::FIELD_SCORE(), NULL};
        m_sort->setSort(sort);
    } else if (m_cluceneQuery->sort == CLuceneQuery::DeathBookPage) {
        SortField *sort[] = {new SortField(AUTHOR_DEATH_FIELD), new SortField(BOOK_ID_FIELD), new SortField(PAGE_ID_FIELD), NULL};
        m_sort->setSort(sort);
    } else {
        SortField *sort[] = {SortField::FIELD_SCORE(), NULL};
        m_sort->setSort(sort);
    }
}

void LibrarySearcher::search()
{
    emit startSearching();

    open();
    buildQuery();
    m_resultsHash.clear();

    QTime time;
    time.start();

    m_hits = m_searcher->search(m_query, m_sort);

    m_timeSearch = time.elapsed();

    m_pageCount = ceil((resultsCount()/(double)m_resultParPage));
    m_currentPage = 0;

    m_stop = false;

    emit doneSearching();
}

class ResultReader
{
public:
    ResultReader(LibrarySearcher *searcher) : m_searcher(searcher) { }

    typedef void result_type;

    void operator()(int &rid)
    {
        m_searcher->fetechResult(rid);
    }

    LibrarySearcher *m_searcher;
};

void LibrarySearcher::fetech()
{
    emit startFeteching();

    m_stop = false;

    QList<int> results = resultsToFetch();
    QtConcurrent::blockingMap(results, ResultReader(this));

    emit doneFeteching();
}

void LibrarySearcher::fetechResult(int rid)
{
    if(m_stop)
        return;

    SearchResult *savedResult = m_resultsHash.object(rid);

    if(savedResult) {
        emit gotResult(savedResult);
        return;
    }

    bool searchIsInTitle = (m_cluceneQuery->searchField == "title");

    Document &doc = m_hits->doc(rid);
    int entryID = Utils::CLucene::WCharToInt(doc.get(PAGE_ID_FIELD));
    int bookID = Utils::CLucene::WCharToInt(doc.get(BOOK_ID_FIELD));
    int score = (int) (m_hits->score(rid) * 100.0);

    LibraryBook::Ptr book = m_libraryManager->bookManager()->getLibraryBook(bookID);

    if(!book) {
        qCritical("LibrarySearcher::fetech: No book with id %d where found", bookID);
        return;
    }

    BookPage *page = new BookPage();
    page->pageID = entryID;

    if(!book->isQuran()) {
        page->titleID = Utils::CLucene::WCharToInt(doc.get(TITLE_ID_FIELD));
    }

    bool gotPage = m_resultReader->getBookPage(book, page);

    if(gotPage) {
        SearchResult *result = new SearchResult(book, page);
        if(searchIsInTitle) {
            result->page->title = Utils::CLucene::highlightText(page->title, m_cluceneQuery, true);
            result->snippet = Utils::String::abbreviate(page->text, 120);
        } else {
            result->snippet = Utils::CLucene::highlightText(page->text, m_cluceneQuery, true);

            if(result->snippet.endsWith("</p"))
                result->snippet.append('>');
        }

        result->resultID = rid;
        result->score = score;
        result->toHtml();

        m_resultsHash.insert(rid, result);

        if(!m_stop)
            emit gotResult(result);

    } else {
        qWarning("LibrarySearcher::fetech No result found for id %d book %d", entryID, bookID);
    }
}

void LibrarySearcher::setQuery(CLuceneQuery *query)
{
    ml_return_on_fail2(query->searchQuery, "LibrarySearcher::setQuery query is null");

    m_cluceneQuery = query;
}

SearchResult *LibrarySearcher::getResult(int resultD)
{
    return m_resultsHash.object(resultD);
}

CLuceneQuery *LibrarySearcher::getSearchQuery()
{
    return m_cluceneQuery;
}

SearchResultReader *LibrarySearcher::getResultReader()
{
    return m_resultReader;
}

int LibrarySearcher::pageCount()
{
    return m_pageCount;
}

int LibrarySearcher::currentPage()
{
    return m_currentPage;
}

int LibrarySearcher::resultsCount()
{
    return m_hits ? m_hits->length() : 0;
}

int LibrarySearcher::searchTime()
{
    return m_timeSearch;
}

int LibrarySearcher::resultsPeerPage()
{
    return m_resultParPage;
}

int LibrarySearcher::fetchStart()
{
    return m_currentPage * m_resultParPage;
}

int LibrarySearcher::fetchEnd()
{
    return qMin(fetchStart()+m_resultParPage, resultsCount());
}

QList<int> LibrarySearcher::resultsToFetch()
{
    QList<int> list;
    int start = fetchStart();
    int end = fetchEnd();

    for(int i=start; i < end; i++)
        list.append(i);

    return list;
}

void LibrarySearcher::nextPage()
{
    if(!isRunning()) {
        if(m_currentPage+1 < pageCount()) {
            m_currentPage++;
            m_action = FETECH;

            start();
        }
    }
}

void LibrarySearcher::prevPage()
{
    if(!isRunning()) {
        if(m_currentPage-1 >= 0) {
            m_currentPage--;
            m_action = FETECH;

            start();
        }
    }
}

void LibrarySearcher::firstPage()
{
    if(!isRunning()) {
        m_currentPage=0;
        m_action = FETECH;

        start();
    }
}

void LibrarySearcher::lastPage()
{
    if(!isRunning()) {
        m_currentPage = pageCount()-1;
        m_action = FETECH;

        start();
    }
}

void LibrarySearcher::page(int page)
{
    if(!isRunning()) {
        if(page < pageCount()) {
            m_currentPage = page;
            m_action = FETECH;

            start();
        }
    }
}
