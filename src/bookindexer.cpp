#include "bookindexer.h"
#include "bookexception.h"
#include "clconstants.h"
#include "mainwindow.h"
#include "clutils.h"

BookIndexer::BookIndexer(QObject *parent) :
    QThread(parent),
    m_writer(0),
    m_trackerIter(0),
    m_stop(false)
{
}

void BookIndexer::setWirter(IndexWriter *writer)
{
    m_writer = writer;
}

void BookIndexer::setTaskIter(IndexTaskIter *iter)
{
    m_trackerIter = iter;
}

void BookIndexer::run()
{
    startIndexing();
}

void BookIndexer::startIndexing()
{
    IndexTask *task = m_trackerIter->next();

    while(task && !m_stop) {
        try {
            task->book = MW->libraryManager()->getBookInfo(task->bookID);

            if(task->book) {
                indexBook(task);
            }
        } catch (BookException &e) {
            qCritical() << "Indexing error:" << e.what();
        }

        task = m_trackerIter->next();
    }

    emit doneIndexing();
}

void BookIndexer::indexBook(IndexTask *task)
{
    TextBookReader *reader = task->reader;
    BookPage *page = reader->page();

    Document doc;
    int tokenAndNoStore = Field::STORE_NO | Field::INDEX_TOKENIZED;
    int storeAndNoToken = Field::STORE_YES | Field::INDEX_UNTOKENIZED;
    wchar_t bookID[128];
    wchar_t pageID[128];
    wchar_t *text = NULL;

    reader->setBookInfo(task->book);
    reader->setLibraryManager(MW->libraryManager());

    reader->openBook(true);
    reader->goFirst();

    _itow(task->bookID, bookID, 10);

    while (reader->hasNext()) {

        reader->nextPage();

        _itow(page->pageID, pageID, 10);
        text = Utils::QStringToWChar(reader->text());

        doc.add( *_CLNEW Field(PAGE_ID_FIELD, pageID, storeAndNoToken));
        doc.add( *_CLNEW Field(BOOK_ID_FIELD, bookID, storeAndNoToken));
        doc.add( *_CLNEW Field(PAGE_TEXT_FIELD, text, tokenAndNoStore, false));

        m_writer->addDocument(&doc);
        doc.clear();
    }

    emit taskDone(task);
}
