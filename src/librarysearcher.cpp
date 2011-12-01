#include "librarysearcher.h"
#include "mainwindow.h"
#include "clutils.h"
#include "clconstants.h"
#include "librarybook.h"
#include "utils.h"
#include "abstractbookreader.h"
#include <qdatetime.h>
#include <qsqlquery.h>
#include <qsettings.h>

LibrarySearcher::LibrarySearcher(QObject *parent)
    : QThread(parent),
      m_action(SEARCH),
      m_searcher(0),
      m_hits(0),
      m_searchQuery(0),
      m_filterQuery(0),
      m_query(0)
{
    m_libraryInfo = MW->libraryInfo();
    m_libraryManager = MW->libraryManager();

    QSettings settings;
    m_resultParPage = settings.value("Search/resultPeerPage", 10).toInt();
}

LibrarySearcher::~LibrarySearcher()
{
    if(m_hits)
        delete m_hits;

    if(m_query)
        delete m_query;

    if(m_filterQuery)
        delete m_filterQuery;

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
        qCritical("Search error(%d): %s", e.number(), e.what());
        emit gotException(e.what(), e.number());
    } catch(std::exception &e){
        emit gotException(e.what(), 0);
    } catch(...) {
        qCritical("Unknow error when searching at \"%s\".",
                  qPrintable(m_libraryInfo->indexDataDir()));
        emit gotException("UNKNOW", -1);
    }
}

void LibrarySearcher::open()
{
    if(!m_searcher) {
        m_searcher = new IndexSearcher(qPrintable(m_libraryInfo->indexDataDir()));
    }
}

void LibrarySearcher::buildQuery()
{
    BooleanQuery *booleanQuery = new BooleanQuery;
    booleanQuery->add(m_searchQuery, BooleanClause::MUST);

    if(m_filterQuery)
        booleanQuery->add(m_filterQuery, m_filterClause);

//    m_query = m_searcher->rewrite(booleanQuery);
    m_query = m_searcher->rewrite(booleanQuery);

    wchar_t *queryText = m_query->toString(PAGE_TEXT_FIELD);
    qDebug() << "Search query:" << Utils::WCharToString(queryText);

    free(queryText);
//    delete booleanQuery;
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

    m_hits = m_searcher->search(m_query);

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

    for(int i=start; i < maxResult;i++){

        SearchResult *savedResult = m_resultsHash.value(i, 0);
        if(savedResult) {
            emit gotResult(savedResult);
            continue;
        }

        Document &doc = m_hits->doc(i);
        int entryID = Utils::WCharToInt(doc.get(PAGE_ID_FIELD));
        int bookID = Utils::WCharToInt(doc.get(BOOK_ID_FIELD));
        int score = (int) (m_hits->score(i) * 100.0);

        LibraryBook *book = m_libraryManager->getBookInfo(bookID);

        if(!book) {
            qCritical("LibrarySearcher::fetech: No book with id %d where found", bookID);
            continue;
        }

        BookPage *page = AbstractBookReader::getBookPage(book, entryID);

        if(page) {
            SearchResult *result = new SearchResult(book, page);
            result->snippet = Utils::highlightText(page->text, m_searchQuery, true);
            result->resultID = i;
            result->score = score;

            m_resultsHash.insert(i, result);

            emit gotResult(result);
        } else {
            qWarning("No result found for id %d book %d", entryID, bookID);
        }
    }

    emit doneFeteching();
}

void LibrarySearcher::setQuery(Query *searchQuery, Query *filterQuery, BooleanClause::Occur filterClause)
{
    Q_CHECK_PTR(searchQuery);

    m_searchQuery = searchQuery;
    m_filterQuery = filterQuery;
    m_filterClause = filterClause;
}

SearchResult *LibrarySearcher::getResult(int resultD)
{
    return m_resultsHash.value(resultD, 0);
}

Query *LibrarySearcher::getSearchQuery()
{
    return m_searchQuery;
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
