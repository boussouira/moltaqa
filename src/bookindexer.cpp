#include "bookindexer.h"
#include "bookexception.h"
#include "clconstants.h"
#include "mainwindow.h"
#include "clutils.h"
#include "textsimplebookreader.h"
#include "textquranreader.h"
#include "texttafessirreader.h"
#include <exception>

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

void BookIndexer::stop()
{
    m_stop = true;
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
        } catch (std::exception &e) {
            qCritical() << "Indexing std error:" << e.what();
        }

        task = m_trackerIter->next();
    }

    emit doneIndexing();
}

void BookIndexer::indexBook(IndexTask *task)
{
    if(task->book->isNormal()) {
        indexSimpleBook(task);
    } else if (task->book->isTafessir()) {
        indexTaffesirBook(task);
    } else if (task->book->isQuran()) {
        indexQuran(task);
    } else {
        qWarning("indexBook: Unknow book type");
        return;
    }

    emit taskDone(task);
}

void BookIndexer::indexQuran(IndexTask *task)
{
    TextQuranReader reader;

    Document doc;
    int tokenAndNoStore = Field::STORE_NO | Field::INDEX_TOKENIZED;
    int storeAndNoToken = Field::STORE_YES | Field::INDEX_UNTOKENIZED;
    wchar_t *bookID;
    wchar_t *pageID;
    wchar_t *soraNumber;
    wchar_t *text = NULL;

    reader.setBookInfo(task->book);

    reader.openBook();
    reader.firstPage();

    BookPage *page = reader.page();

    bookID = Utils::intToWChar(task->bookID);

    while (reader.hasNext()) {
        reader.nextPage();

        pageID = Utils::intToWChar(page->pageID);
        text = Utils::QStringToWChar(page->text);
        soraNumber = Utils::intToWChar(page->sora);

        doc.add( *_CLNEW Field(BOOK_ID_FIELD, bookID, storeAndNoToken));
        doc.add( *_CLNEW Field(PAGE_ID_FIELD, pageID, storeAndNoToken, false));
        doc.add( *_CLNEW Field(PAGE_TEXT_FIELD, text, tokenAndNoStore, false));
        doc.add( *_CLNEW Field(QURAN_SORA_FIELD, soraNumber, tokenAndNoStore, false));

        m_writer->addDocument(&doc);
        doc.clear();
    }

    free(bookID);
}

void BookIndexer::indexSimpleBook(IndexTask *task)
{
    TextSimpleBookReader reader;

    Document doc;
    int tokenAndNoStore = Field::STORE_NO | Field::INDEX_TOKENIZED;
    int storeAndNoToken = Field::STORE_YES | Field::INDEX_UNTOKENIZED;
    wchar_t *bookID;
    wchar_t *pageID;
    wchar_t *titleID;
    wchar_t *text;
    //wchar_t *title;

    reader.setBookInfo(task->book);

    reader.openBook();
    reader.load();
    reader.firstPage();

    BookPage *page = reader.page();

    bookID = Utils::intToWChar(task->bookID);

    while (reader.hasNext()) {
        reader.nextPage();

        pageID = Utils::intToWChar(page->pageID);
        text = Utils::QStringToWChar(page->text);
        titleID = Utils::intToWChar(page->titleID);

        doc.add( *_CLNEW Field(BOOK_ID_FIELD, bookID, storeAndNoToken));
        doc.add( *_CLNEW Field(PAGE_ID_FIELD, pageID, storeAndNoToken, false));
        doc.add( *_CLNEW Field(TITLE_ID_FIELD, titleID, storeAndNoToken, false));
        doc.add( *_CLNEW Field(PAGE_TEXT_FIELD, text, tokenAndNoStore, false));

        m_writer->addDocument(&doc);
        doc.clear();
    }

    free(bookID);
}

void BookIndexer::indexTaffesirBook(IndexTask *task)
{
    TextTafessirReader reader;

    Document doc;
    int tokenAndNoStore = Field::STORE_NO | Field::INDEX_TOKENIZED;
    int storeAndNoToken = Field::STORE_YES | Field::INDEX_UNTOKENIZED;
    wchar_t *bookID;
    wchar_t *pageID;
    wchar_t *titleID;
    wchar_t *text;
    //wchar_t *title;

    reader.setBookInfo(task->book);

    reader.openBook();
    reader.load();
    reader.firstPage();

    BookPage *page = reader.page();

    bookID = Utils::intToWChar(task->bookID);

    while (reader.hasNext()) {
        reader.nextPage();

        pageID = Utils::intToWChar(page->pageID);
        text = Utils::QStringToWChar(page->text);
        titleID = Utils::intToWChar(page->titleID);

        doc.add( *_CLNEW Field(BOOK_ID_FIELD, bookID, storeAndNoToken));
        doc.add( *_CLNEW Field(PAGE_ID_FIELD, pageID, storeAndNoToken, false));
        doc.add( *_CLNEW Field(TITLE_ID_FIELD, titleID, storeAndNoToken, false));
        doc.add( *_CLNEW Field(PAGE_TEXT_FIELD, text, tokenAndNoStore, false));

        m_writer->addDocument(&doc);
        doc.clear();
    }

    free(bookID);
}
