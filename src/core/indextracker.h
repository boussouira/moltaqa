#ifndef INDEXTRACKER_H
#define INDEXTRACKER_H

#include "indextaskiter.h"
#include "xmldomhelper.h"

#include <qdom.h>
#include <qobject.h>
#include <qrunnable.h>

class LibraryInfo;
class LibraryManager;
class LibraryBookManager;

class IndexTracker : public QObject, public QRunnable
{
    Q_OBJECT
public:
    IndexTracker(QObject *parent = 0);
    ~IndexTracker();

    static IndexTracker *instance();

    void addTask(int bookID, IndexTask::Task task, bool emitSignal=true);
    void addTask(const QList<int> &books, IndexTask::Task task);
    void removeTask(IndexTask *task);

    bool contains(IndexTask *task);
    int taskCount();
    IndexTaskIter* getTaskIter();

    void loadTask();
    void save();

    void findTasks();
    void run();

protected:
    void open();
    void addTask(IndexTask *task);
    void deleteTask(IndexTask *task);

signals:
    void gotTask();

protected:
    LibraryInfo *m_libraryInfo;
    LibraryManager *m_libraryManager;
    LibraryBookManager *m_bookManager;
    QList<IndexTask*> m_tasks;
    QString m_trackerFile;
    XmlDomHelper m_dom;
};

#endif // INDEXTRACKER_H
