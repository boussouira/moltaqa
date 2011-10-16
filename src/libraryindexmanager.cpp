#include "libraryindexmanager.h"
#include <QSettings>
#include <QDir>
#include <QThread>

#include "mainwindow.h"
#include "arabicanalyzer.h"

LibraryIndexManager::LibraryIndexManager(QObject *parent) :
    QObject(parent)
{
    m_indexTracker = MW->indexTracker();
    m_library = MW->libraryInfo();
}

bool LibraryIndexManager::openWriter()
{
    QDir dir;
    QSettings settings;
    ArabicAnalyzer *analyzer = new ArabicAnalyzer();

    int ramSize = settings.value("ramSize", 100).toInt();
    m_threadCount = settings.value("threadCount", QThread::idealThreadCount()).toInt();

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

void LibraryIndexManager::start()
{

}

void LibraryIndexManager::stop()
{
}
