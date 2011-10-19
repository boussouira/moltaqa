#include "indexmanager.h"
#include <qsettings.h>
#include <qdir.h>
#include <qthread.h>
#include <qvariant.h>

#include "mainwindow.h"
#include "arabicanalyzer.h"
#include "bookindexer.h"

IndexManager::IndexManager(QObject *parent) :
    QObject(parent)
{
    m_indexTracker = MW->indexTracker();
    m_library = MW->libraryInfo();
}

bool IndexManager::openWriter()
{
    QDir dir;
    QSettings settings;
    ArabicAnalyzer *analyzer = new ArabicAnalyzer();

    int ramSize = settings.value("ramSize", 100).toInt();

    if(!dir.exists(m_library->indexDataDir()))
        dir.mkdir(m_library->indexDataDir());
    if(IndexReader::indexExists(qPrintable(m_library->indexDataDir()))) {
        if(IndexReader::isLocked(qPrintable(m_library->indexDataDir())))
            IndexReader::unlock(qPrintable(m_library->indexDataDir()));

        m_writer = _CLNEW IndexWriter( qPrintable(m_library->indexDataDir()) ,analyzer, false);
    } else {
        m_writer = _CLNEW IndexWriter( qPrintable(m_library->indexDataDir()) ,analyzer, true);
    }

    m_writer->setUseCompoundFile(false);
    m_writer->setMaxFieldLength(IndexWriter::DEFAULT_MAX_FIELD_LENGTH);
    m_writer->setRAMBufferSizeMB(ramSize);
    m_writer->setMergeFactor(25);

    return true;
}

void IndexManager::start()
{
    if(!openWriter()) {
        qWarning("Can't open IndexWriter");
        return;
    }

    QSettings settings;
    m_threadCount = settings.value("threadCount", QThread::idealThreadCount()).toInt();
    m_taskIter = m_indexTracker->getTaskIter();
    m_indexedBookCount = 0;

    m_threads.clear();
    m_indexingTime.start();

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

void IndexManager::stop()
{
}

void IndexManager::taskDone(IndexTask *task)
{
    qDebug() << "Indexed:" << task->book->bookDisplayName;
    m_indexTracker->removeTask(task);
    emit progress(++m_indexedBookCount, m_taskIter->taskCount());
}

void IndexManager::threadDoneIndexing()
{
    if(--m_threadCount <= 0) {
        m_writer->close();
        _CLLDELETE(m_writer);
        qDebug("Done indexing %d ms", m_indexingTime.elapsed());

        delete m_taskIter;
        qDeleteAll(m_threads);
        m_threads.clear();

        m_indexTracker->flush();

        emit done();
    }
}
