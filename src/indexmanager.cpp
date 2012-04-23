#include "indexmanager.h"
#include <qsettings.h>
#include <qdir.h>
#include <qthread.h>
#include <qvariant.h>

#include "mainwindow.h"
#include "arabicanalyzer.h"
#include "bookindexer.h"
#include "utils.h"
#include "timeutils.h"

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

    qDebug("IndexManager: open writer using %d MB", ramSize);

    return true;
}

bool IndexManager::isIndexing()
{
    return !m_threads.isEmpty();
}

void IndexManager::start()
{
    ML_ASSERT2(openWriter(), "IndexManager: Can't open IndexWriter");

    QSettings settings;
    m_threadCount = settings.value("Search/threadCount", QThread::idealThreadCount()).toInt();
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
    qDebug() << "IndexManager::taskDone" << (task->task==IndexTask::Add
                 ? "Add"
                 : (task->task==IndexTask::Delete
                    ? "Delete"
                    : "Update"))
             << task->book->id
             << task->book->title;


    m_indexTracker->removeTask(task);
    emit progress(++m_indexedBookCount, m_taskIter->taskCount());
}

void IndexManager::threadDoneIndexing()
{
    if(--m_threadCount <= 0) {
        if(m_writer) {
            m_writer->close();
            ML_DELETE(m_writer);
        }

        ML_DELETE_CHECK(m_analyzer);
        ML_DELETE_CHECK(m_taskIter);

        qDebug() << "IndexManager:"
                 << tr("تمت الفهرسة خلال %1").arg(Utils::Time::secondsToString(m_indexingTime.elapsed()));

        qDeleteAll(m_threads);
        m_threads.clear();

        m_indexTracker->save();

        emit done();
    }
}
