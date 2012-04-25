#include "librarysearcher.h"
#include "mainwindow.h"
#include "clutils.h"
#include "clconstants.h"
#include "librarybook.h"
#include "utils.h"
#include "stringutils.h"
#include "abstractbookreader.h"
#include "librarybookmanager.h"
#include "searchresultreader.h"

#include <qdatetime.h>
#include <qsqlquery.h>
#include <qsettings.h>

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

    QSettings settings;
    m_resultParPage = settings.value("Search/resultPeerPage", 10).toInt();
}

LibrarySearcher::~LibrarySearcher()
{
    ML_DELETE_CHECK(m_hits);
    ML_DELETE_CHECK(m_query);
    ML_DELETE_CHECK(m_sort);
    ML_DELETE_CHECK(m_cluceneQuery);
    ML_DELETE_CHECK(m_resultReader);

    if(m_searcher) {
        m_searcher->close();
        delete m_searcher;
    }

    qDeleteAll(m_resultsHash);
    m_resultsHash.clear();
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
    BooleanQuery booleanQuery;
    booleanQuery.add(m_cluceneQuery->searchQuery, BooleanClause::MUST);

    if(m_cluceneQuery->filterQuery)
        booleanQuery.add(m_cluceneQuery->filterQuery, m_cluceneQuery->filterClause);

    m_query = m_searcher->rewrite(&booleanQuery);

    wchar_t *queryText = m_query->toString(PAGE_TEXT_FIELD);
    qDebug() << "LibrarySearcher: Search query:" << Utils::CLucene::WCharToString(queryText);

    free(queryText);

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
    qDeleteAll(m_resultsHash);
    m_resultsHash.clear();

    QTime time;
    time.start();

    m_hits = m_searcher->search(m_query, m_sort);

    m_timeSearch = time.elapsed();

    m_pageCount = ceil((resultsCount()/(double)m_resultParPage));
    m_currentPage = 0;

    emit doneSearching();
}

void LibrarySearcher::fetech()
{
    emit startFeteching();

    int start = m_currentPage * m_resultParPage;
    int maxResult  = qMin(start+m_resultParPage, resultsCount());
    bool searchIsInTitle = (m_cluceneQuery->searchField == "title");

    for(int i=start; i < maxResult;i++){

        SearchResult *savedResult = m_resultsHash.value(i, 0);
        if(savedResult) {
            emit gotResult(savedResult);
            continue;
        }

        Document &doc = m_hits->doc(i);
        int entryID = Utils::CLucene::WCharToInt(doc.get(PAGE_ID_FIELD));
        int bookID = Utils::CLucene::WCharToInt(doc.get(BOOK_ID_FIELD));
        int score = (int) (m_hits->score(i) * 100.0);

        LibraryBookPtr book = m_libraryManager->bookManager()->getLibraryBook(bookID);

        if(!book) {
            qCritical("LibrarySearcher::fetech: No book with id %d where found", bookID);
            continue;
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
            }

            result->resultID = i;
            result->score = score;

            m_resultsHash.insert(i, result);

            if(m_stop) {
                m_stop = false;
                return;
            } else {
                emit gotResult(result);
            }
        } else {
            qWarning("LibrarySearcher::fetech No result found for id %d book %d", entryID, bookID);
        }
    }

    emit doneFeteching();
}

void LibrarySearcher::setQuery(CLuceneQuery *query)
{
    ML_ASSERT2(query->searchQuery, "LibrarySearcher::setQuery query is null");

    m_cluceneQuery = query;
}

SearchResult *LibrarySearcher::getResult(int resultD)
{
    return m_resultsHash.value(resultD, 0);
}

CLuceneQuery *LibrarySearcher::getSearchQuery()
{
    return m_cluceneQuery;
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
    return m_hits->length();
}

int LibrarySearcher::searchTime()
{
    return m_timeSearch;
}

int LibrarySearcher::resultsPeerPage()
{
    return m_resultParPage;
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

void LibrarySearcher::fetechResults(int page)
{
    if(!isRunning()) {
        if(page < pageCount()) {
            m_currentPage = page;
            m_action = FETECH;

            start();
        }
    }
}
