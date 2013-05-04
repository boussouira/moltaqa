#include "indextaskiter.h"

IndexTaskIter::IndexTaskIter()
{
    m_currentTask = -1;
}

IndexTaskIter::~IndexTaskIter()
{
    qDeleteAll(m_tasks);
    m_tasks.clear();
}

bool IndexTask::operator==(const IndexTask &other)  const
{
    return bookID == other.bookID && task == other.task;
}

IndexTask::Task IndexTask::stringToTask(QString task)
{
    if(task.compare("add", Qt::CaseInsensitive) == 0)
        return Add;
    else if(task.compare("delete", Qt::CaseInsensitive) == 0)
        return Delete;
    else if(task.compare("update", Qt::CaseInsensitive) == 0)
        return Update;
    else
        return Unknow;
}

QString IndexTask::taskToString(const IndexTask::Task &task)
{
    switch(task) {
    case Add:
        return "Add";
    case Delete:
        return "Delete";
    case Update:
        return "Update";
    default:
        return "Unknow";
    }
}

void IndexTaskIter::addTask(IndexTask *task)
{
    if(!contains(task))
        m_tasks.append(task);
}

void IndexTaskIter::removeTask(IndexTask *task)
{
    m_tasks.removeAll(task);
}

bool IndexTaskIter::contains(IndexTask *task)
{
    foreach (IndexTask *t, m_tasks) {
        if(t->bookID == task->bookID && t->task == task->task)
            return true;
    }

    return false;
}

IndexTask *IndexTaskIter::next()
{
    QMutexLocker locker(&m_mutex);

    m_currentTask++;
    if(m_currentTask < m_tasks.size())
        return m_tasks.at(m_currentTask);
    else
        return 0;
}

int IndexTaskIter::taskCount()
{
    return m_tasks.size();
}

QDebug& operator <<(QDebug &dbg, const IndexTask &task)
{
    dbg.nospace() << "(Book: " << task.bookID
                  << ", Task: " << qPrintable(IndexTask::taskToString(task.task))
                  << ")";
    return dbg.space();
}

QDebug& operator <<(QDebug &dbg, IndexTask *task)
{
    return dbg << *task;
}
