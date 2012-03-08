#include "indexmanager.h"
#include <qsettings.h>
#include <qdir.h>
#include <qthread.h>
#include <qvariant.h>

#include "mainwindow.h"
#include "arabicanalyzer.h"
#include "bookindexer.h"
#include "utils.h"

IndexManager::IndexManager(QObject *parent) :
    QObject(parent)
{
    m_indexTracker = IndexTracker::instance();
    m_library = MW->libraryInfo();

    m_taskIter = 0;
    m_analyzer = 0;
}

bool IndexManager::openWriter()
{
    QDir dir;
    QSettings settings;
    m_analyzer = new ArabicAnalyzer();

    int ramSize = settings.value("Search/ramSize", 100).toInt();

    if(!dir.exists(m_library->indexDataDir()))
        dir.mkdir(m_library->indexDataDir());
    if(IndexReader::indexExists(qPrintable(m_library->indexDataDir()))) {
        if(IndexReader::isLocked(qPrintable(m_library->indexDataDir())))
            IndexReader::unlock(qPrintable(m_library->indexDataDir()));

        m_writer = _CLNEW IndexWriter( qPrintable(m_library->indexDataDir()), m_analyzer, false);
    } else {
        m_writer = _CLNEW IndexWriter( qPrintable(m_library->indexDataDir()), m_analyzer, true);
    }

    m_writer->setUseCompoundFile(false);
    m_writer->setMaxFieldLength(IndexWriter::DEFAULT_MAX_FIELD_LENGTH);
    m_writer->setRAMBufferSizeMB(ramSize);
    m_writer->setMergeFactor(25);

    return true;
}

bool IndexManager::isIndexing()
{
    return !m_threads.isEmpty();
}

void IndexManager::start()
{
    if(!openWriter()) {
        qWarning("Can't open IndexWriter");
        return;
    }

    QSettings settings;
    m_threadCount = settings.value("Search/threadCount", QThread::idealThreadCount()).toInt();
    m_taskIter = m_indexTracker->getTaskIter();
    m_indexedBookCount = 0;

    m_threads.clear();
    m_indexingTime.start();

    if(m_taskIter->taskCount()) {
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
    qDebug("Stop indexer...");
    foreach (BookIndexer *thread, m_threads) {
            thread->stop();
    }

    foreach (BookIndexer *thread, m_threads) {
        if(thread->isRunning()) {
            qDebug("Wait for thread to finnish...");
            thread->wait();
        }
    }
}

void IndexManager::taskDone(IndexTask *task)
{
    qDebug() << "Indexer" << (task->task==IndexTask::Add
                 ? "Add"
                 : (task->task==IndexTask::Delete
                    ? "Delete"
                    : "Update"))
             << task->book->bookID
             << task->book->bookDisplayName;


    m_indexTracker->removeTask(task);
    emit progress(++m_indexedBookCount, m_taskIter->taskCount());
}

void IndexManager::threadDoneIndexing()
{
    if(--m_threadCount <= 0) {
        if(m_writer) {
            m_writer->close();
            _CLLDELETE(m_writer);
            m_writer = 0;
        }

        if(m_analyzer) {
            delete m_analyzer;
            m_analyzer = 0;
        }

        qDebug() << tr("تمت الفهرسة خلال %1").arg(Utils::secondsToString(m_indexingTime.elapsed()));

        if(m_taskIter) {
            delete m_taskIter;
            m_taskIter = 0;
        }

        qDeleteAll(m_threads);
        m_threads.clear();

        m_indexTracker->flush();

        emit done();
    }
}
