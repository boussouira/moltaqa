#include "indexmanager.h"
#include "mainwindow.h"
#include "arabicanalyzer.h"
#include "bookindexer.h"
#include "utils.h"
#include "timeutils.h"
#include "clconstants.h"
#include "bookexception.h"

#include <qdir.h>
#include <qthread.h>
#include <qvariant.h>

IndexManager::IndexManager(QObject *parent) :
    QObject(parent),
    m_library(MW->libraryInfo()),
    m_writer(0),
    m_indexTracker(IndexTracker::instance()),
    m_taskIter(0),
    m_analyzer(0),
    m_booksCount(0),
    m_indexedBooks(0),
    m_threadCount(0),
    m_indexedBookCount(0)
{
}

bool IndexManager::openWriter()
{
    ml_return_val_on_fail2(!m_writer, "IndexManager::openWriter already open", true);

    QDir dir;
    m_analyzer = new ArabicAnalyzer();

    int ramSize = Utils::Settings::get("Search/ramSize", DEFAULT_INDEXING_RAM).toInt();

    if(!dir.exists(m_library->indexDataDir()))
        dir.mkdir(m_library->indexDataDir());

    try {
        if(IndexReader::indexExists(qPrintable(m_library->indexDataDir()))) {
            if(IndexReader::isLocked(qPrintable(m_library->indexDataDir())))
                IndexReader::unlock(qPrintable(m_library->indexDataDir()));

            m_writer = new IndexWriter( qPrintable(m_library->indexDataDir()), m_analyzer, false);
        } else {
            m_writer = new IndexWriter( qPrintable(m_library->indexDataDir()), m_analyzer, true);
        }

        m_writer->setUseCompoundFile(false);
        m_writer->setMaxFieldLength(IndexWriter::DEFAULT_MAX_FIELD_LENGTH);
        m_writer->setRAMBufferSizeMB(ramSize);
        m_writer->setMergeFactor(25);

        qDebug("IndexManager: open writer using %d MB", ramSize);

        return true;
    } catch(CLuceneError &e) {
        qCritical("IndexManager::openWriter CLucene exception %s", e.what());
        return false;
    } catch(std::exception &e){
        qCritical("IndexManager::openWriter STD exception %s", e.what());
        return false;
    } catch(...) {
        qCritical() << "IndexManager::openWriter Unknow exception when opening index"
                    << m_library->indexDataDir();

        return false;
    }
}

bool IndexManager::isIndexing()
{
    return m_threads.size();
}

void IndexManager::start()
{
    ml_return_on_fail2(openWriter(), "IndexManager: Can't open IndexWriter");

    m_threadCount = Utils::Settings::get("Search/threadCount", QThread::idealThreadCount()).toInt();
    m_taskIter = m_indexTracker->getTaskIter();
    m_indexedBookCount = 0;

    m_threads.clear();
    m_indexingTime.start();

    if(m_taskIter->taskCount()) {
        m_threadCount = qMin(m_threadCount, m_taskIter->taskCount());

        qDebug("IndexManager: start %d tasks with %d threads",
               m_taskIter->taskCount(), m_threadCount);

        for(int i=0;i<m_threadCount;i++) {
            BookIndexer *indexThread = new BookIndexer();
            connect(indexThread, SIGNAL(taskDone(IndexTask*)), SLOT(taskDone(IndexTask*)));
            connect(indexThread, SIGNAL(doneIndexing()), SLOT(threadDoneIndexing()));

            indexThread->setTaskIter(m_taskIter);
            indexThread->setWirter(m_writer);

            indexThread->start();
            m_threads.append(indexThread);
        }

        emit started();
    }
}

void IndexManager::stop()
{
    qDebug("IndexManager: Stop indexer...");
    foreach (BookIndexer *thread, m_threads) {
            thread->stop();
    }

    foreach (BookIndexer *thread, m_threads) {
        if(thread->isRunning()) {
            qDebug("IndexManager: Wait for thread to finnish...");
            thread->wait();
        }
    }
}

void IndexManager::taskDone(IndexTask *task)
{
    m_indexTracker->removeTask(task);
    emit progress(++m_indexedBookCount, m_taskIter->taskCount());
}

void IndexManager::threadDoneIndexing()
{
    if(--m_threadCount <= 0) {
        if(m_writer) {
            m_writer->close();
            ml_delete(m_writer);
        }

        ml_delete_check(m_analyzer);
        ml_delete_check(m_taskIter);

        qDebug() << "IndexManager:"
                 << tr("تمت الفهرسة خلال %1").arg(Utils::Time::secondsToString(m_indexingTime.elapsed()));

        qDeleteAll(m_threads);
        m_threads.clear();

        m_indexTracker->save();

        emit done();
    }
}

bool IndexManager::optimize()
{
    ml_return_val_on_fail2(!isIndexing(), "IndexManager::optimize can't optimize when indexing", false);
    ml_return_val_on_fail2(openWriter(), "IndexManager::optimize Can't open IndexWriter", false);

    try {
        m_writer->optimize();
        return true;
    } catch(CLuceneError &err) {
        qCritical("IndexManager::optimize CLucene Error: %s", err.what());
        return false;
    } catch(std::exception &err){
        qCritical("IndexManager::optimize STD error: %s", err.what());
        return false;
    } catch(...){
        qCritical("IndexManager::optimize Unkonw error");
        return false;
    }
}
