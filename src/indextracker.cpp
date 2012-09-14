#include "indextracker.h"
#include "mainwindow.h"
#include "bookexception.h"
#include "librarybookmanager.h"
#include "utils.h"
#include "xmlutils.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qdebug.h>

bool operator ==(const QDomElement &node, IndexTask *task)
{
    return (node.attribute("book").toInt() == task->bookID &&
            IndexTask::stringToTask(node.attribute("type")) == task->task);
}

static IndexTracker *m_instance = 0;

IndexTracker::IndexTracker(QObject *parent) :
    QObject(parent)
{
    m_libraryInfo = MW->libraryInfo();
    m_libraryManager = LibraryManager::instance();
    m_bookManager = m_libraryManager->bookManager();

    setAutoDelete(false);

    try {
        open();
        loadTask();
    } catch(BookException &e) {
        qCritical() << "IndexTracker: error" << e.what();
    }

    ml_set_instance(m_instance, this);
}

IndexTracker::~IndexTracker()
{
    qDeleteAll(m_tasks);
    m_tasks.clear();

    m_instance = 0;
}

IndexTracker* IndexTracker::instance()
{
    return m_instance;
}

void IndexTracker::open()
{
    m_trackerFile = m_libraryInfo->trackerFile();

    if(!QFile::exists(m_trackerFile))
        throw BookException(tr("لم يتم العثور على الملف:"), m_trackerFile);

    m_dom.setFilePath(m_trackerFile);
    m_dom.load();
}

void IndexTracker::loadTask()
{
    qDeleteAll(m_tasks);
    m_tasks.clear();

    QDomElement taskNode = m_dom.rootElement().firstChildElement("task");

    while(!taskNode.isNull()) {
        IndexTask *task = new IndexTask();
        task->bookID = taskNode.attribute("book").toInt();
        task->task = IndexTask::stringToTask(taskNode.attribute("type"));
        m_tasks.append(task);

        taskNode = taskNode.nextSiblingElement("task");
    }
}

void IndexTracker::addTask(IndexTask *task)
{
    if(!contains(task)) {

        QDomElement newTask = m_dom.domDocument().createElement("task");

        newTask.setAttribute("book", task->bookID);
        newTask.setAttribute("type", IndexTask::taskToString(task->task));
        m_dom.rootElement().appendChild(newTask);
        m_dom.setNeedSave(true);

        m_tasks.append(task);

    } else {
        if(!m_tasks.contains(task))
            delete task;
    }
}

void IndexTracker::addTask(int bookID, IndexTask::Task task, bool emitSignal)
{
    IndexTask *t = new IndexTask();
    t->bookID = bookID;
    t->task = task;

    addTask(t);

    if(emitSignal)
        emit gotTask();
}

void IndexTracker::addTask(const QList<int> &books, IndexTask::Task task)
{
    if(books.size()) {

        foreach(int bookID, books) {
            addTask(bookID, task, false);
        }

        qDebug("IndexTracker: Got %d books to %s",
               books.size(),
               qPrintable(IndexTask::taskToString(task)));
    }
}

void IndexTracker::removeTask(IndexTask *task)
{
    if(contains(task)) {
        QDomNodeList nodesList = m_dom.rootElement().elementsByTagName("task");

        for (int i=0; i<nodesList.count(); i++) {
            QDomElement indexElement = nodesList.at(i).toElement();

            if(indexElement == task) {
                m_dom.rootElement().removeChild(indexElement);
                m_dom.setNeedSave(true);
                deleteTask(task);

                if(task->task != IndexTask::Delete)
                    m_bookManager->setBookIndexStat(task->bookID, LibraryBook::Indexed);

                break;
            }
        }
    } else {
        qDebug() << "Task" << task << "doesn't exist";
    }
}

bool IndexTracker::contains(IndexTask *task)
{
    foreach (IndexTask *t, m_tasks) {
        if(t->bookID == task->bookID && t->task == task->task)
            return true;
    }

    return false;
}

void IndexTracker::save()
{
    m_dom.save();
}

void IndexTracker::findTasks()
{
    QStringList indexFiles = QDir(m_libraryInfo->indexDataDir()).entryList(QDir::Files|QDir::NoDotAndDotDot);
    if(indexFiles.isEmpty() && m_bookManager->booksCount()) {
        qWarning("IndexTracker::findTasks index data directory is empty");
        m_bookManager->setBookIndexStat(-1, LibraryBook::NotIndexed);
    }

    QThreadPool::globalInstance()->start(this);
}

void IndexTracker::run()
{
    addTask(m_bookManager->getBooksWithIndexStat(LibraryBook::NotIndexed), IndexTask::Add);
    addTask(m_bookManager->getBooksWithIndexStat(LibraryBook::Delete), IndexTask::Delete);
    addTask(m_bookManager->getBooksWithIndexStat(LibraryBook::Update), IndexTask::Update);

    if(m_tasks.size()) {
        m_dom.maySave();
        emit gotTask();
    }
}

int IndexTracker::taskCount()
{
    return m_tasks.count();
}

IndexTaskIter* IndexTracker::getTaskIter()
{
    m_dom.reload();
    loadTask();

    IndexTaskIter *iter = new IndexTaskIter();
    int count = m_tasks.size();
    int maxUpdateCount = Utils::Settings::get("Search/maxBookToUpdate", 6000).toInt();

    if(maxUpdateCount)
        count = qMin(count, maxUpdateCount);

    for(int i=0; i<count; i++) {
        IndexTask *t = new IndexTask();
        t->bookID = m_tasks[i]->bookID;
        t->task = m_tasks[i]->task;

        iter->addTask(t);
    }

    return iter;
}

void IndexTracker::deleteTask(IndexTask *task)
{
    for(int i=0; i < m_tasks.size(); i++) {
        IndexTask *t = m_tasks.at(i);
        if(t->bookID == task->bookID && t->task == task->task) {
            delete m_tasks.takeAt(i);
            break;
        }
    }
}
