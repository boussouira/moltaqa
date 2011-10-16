#ifndef INDEXTASKITER_H
#define INDEXTASKITER_H

#include "librarybook.h"
#include <qset.h>
#include <qmutex.h>

class IndexTask {
public:
    IndexTask() {}

    enum Task {
        Unknow,
        Add,
        Delete,
        Update
    };

    int bookID;
    LibraryBook *book;
    Task task;

    bool operator==(const IndexTask &other);
    static Task stringToTask(QString task);
    static QString taskToString(const Task &task);
};

QDebug &operator <<(QDebug &dbg, IndexTask task);

class IndexTaskIter
{
public:
    IndexTaskIter();

    void addTask(IndexTask *task);
    void removeTask(IndexTask *task);
    bool contains(IndexTask *task);
    int taskCount();
    IndexTask *next();

protected:
    QList<IndexTask*> m_tasks;
    int m_currentTask;
    QMutex m_mutex;
};

#endif // INDEXTASKITER_H
