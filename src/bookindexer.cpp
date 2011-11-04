#include "bookindexer.h"
#include "bookexception.h"
#include "clconstants.h"
#include "mainwindow.h"
#include "clutils.h"
#include "textsimplebookreader.h"
#include "textquranreader.h"

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
    // TODO: implement delete and update books
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
    TextBookReader *reader = 0;
    if(task->book->isNormal() || task->book->isTafessir()) {
        reader = new TextSimpleBookReader();
    } else if (task->book->isQuran()) {
        reader = new TextQuranReader();
    } else {
        qWarning("indexBook: Unknow book type");
        return;
    }

    BookPage *page = reader->page();

    Document doc;
    int tokenAndNoStore = Field::STORE_NO | Field::INDEX_TOKENIZED;
    int storeAndNoToken = Field::STORE_YES | Field::INDEX_UNTOKENIZED;
    wchar_t bookID[128];
    wchar_t pageID[128];
    wchar_t *text = NULL;

    reader->setBookInfo(task->book->clone()); // The reader will delete this clone
    reader->setLibraryManager(MW->libraryManager());

    reader->openBook(true);
    reader->goFirst();

    Utils::intToWChar(task->bookID, bookID, 10);

    while (reader->hasNext()) {

        reader->nextPage();

        Utils::intToWChar(page->pageID, pageID, 10);
        text = Utils::QStringToWChar(reader->text());

        doc.add( *_CLNEW Field(PAGE_ID_FIELD, pageID, storeAndNoToken));
        doc.add( *_CLNEW Field(BOOK_ID_FIELD, bookID, storeAndNoToken));
        doc.add( *_CLNEW Field(PAGE_TEXT_FIELD, text, tokenAndNoStore, false));

        m_writer->addDocument(&doc);
        doc.clear();
    }

    emit taskDone(task);

    if(reader)
        delete reader;
}
