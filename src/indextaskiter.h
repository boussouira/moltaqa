#ifndef INDEXTASKITER_H
#define INDEXTASKITER_H

#include "librarybook.h"
#include "textbookreader.h"
#include <qmutex.h>

class IndexTask {
public:
    IndexTask() : bookID(0), book(0), reader(0), task(Unknow)
    {}

    enum Task {
        Unknow,
        Add,
        Delete,
        Update
    };

    int bookID;
    LibraryBookPtr book;
    TextBookReader *reader;
    Task task;

    bool operator==(const IndexTask &other);
    static Task stringToTask(QString task);
    static QString taskToString(const Task &task);
};

QDebug &operator <<(QDebug &dbg, const IndexTask &task);
QDebug &operator <<(QDebug &dbg, IndexTask *task);

class IndexTaskIter
{
public:
    IndexTaskIter();
    ~IndexTaskIter();

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
