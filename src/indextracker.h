#ifndef INDEXTRACKER_H
#define INDEXTRACKER_H

#include <qobject.h>
#include <qdom.h>

#include "libraryinfo.h"
#include "librarymanager.h"
#include "indextaskiter.h"

class IndexTracker : public QObject
{
    Q_OBJECT
public:
    IndexTracker(QObject *parent = 0);
    ~IndexTracker();

    void addTask(IndexTask *task);
    void addTask(int bookID, IndexTask::Task task);
    void addTask(const QList<int> &books, IndexTask::Task task);
    void removeTask(IndexTask *task);

    bool contains(IndexTask *task);
    int taskCount();
    IndexTaskIter* getTaskIter();

    void loadTask();
    void flush();

protected:
    void open();
    void deleteTask(IndexTask *task);

signals:
    void gotTask();

protected:
    LibraryInfo *m_libraryInfo;
    LibraryManager *m_libraryManager;
    QDomDocument m_doc;
    QDomElement m_rootElement;
    QList<IndexTask*> m_tasks;
    QString m_trackerFile;
};

#endif // INDEXTRACKER_H
