#include "indextracker.h"
#include "mainwindow.h"
#include "bookexception.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qdebug.h>

bool operator ==(const QDomElement &node, IndexTask *task)
{
    return (node.attribute("book").toInt() == task->bookID &&
            IndexTask::stringToTask(node.attribute("type")) == task->task);
}

IndexTracker::IndexTracker(QObject *parent) :
    QObject(parent)
{
    m_libraryInfo = MW->libraryInfo();
    m_libraryManager = MW->libraryManager();

    try {
        open();
        loadTask();
    } catch(BookException &e) {
        qCritical() << "IndexTracker:" << e.what();
    }
}

IndexTracker::~IndexTracker()
{
    flush();
}

void IndexTracker::open()
{
    m_trackerFile = m_libraryInfo->trackerFile();

    QString errorStr;
    int errorLine;
    int errorColumn;

    if(!QFile::exists(m_trackerFile))
        throw BookException(tr("لم يتم العثور على الملف:"), m_trackerFile);

    QFile file(m_trackerFile);
    if (!file.open(QIODevice::ReadOnly))
        throw BookException(tr("حدث خطأ أثناء فتح الملف:"), m_trackerFile);

    if (!m_doc.setContent(&file, 0, &errorStr, &errorLine, &errorColumn))
        throw BookException(tr("Parse error at line %1, column %2: %3")
                            .arg(errorLine).arg(errorColumn).arg(errorStr), m_trackerFile);

    m_rootElement = m_doc.documentElement();
}

void IndexTracker::loadTask()
{
    m_tasks.clear();

    QDomElement taskNode = m_rootElement.firstChildElement("task");

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

        QDomElement newTask = m_doc.createElement("task");

        newTask.setAttribute("book", task->bookID);
        newTask.setAttribute("type", IndexTask::taskToString(task->task));
        m_rootElement.appendChild(newTask);

        m_tasks.append(task);

    } else {
        qWarning() << "Task exists:" << task;
    }
}
void IndexTracker::addTask(int bookID, IndexTask::Task task)
{
    IndexTask *t = new IndexTask();
    t->bookID = bookID;
    t->task = task;

    addTask(t);
}

void IndexTracker::addTask(const QList<int> &books, IndexTask::Task task)
{
    foreach(int bookID, books) {
        addTask(bookID, task);
    }

    emit gotTask();
}

void IndexTracker::removeTask(IndexTask *task)
{
    if(contains(task)) {
        QDomNodeList nodesList = m_rootElement.elementsByTagName("task");

        for (int i=0; i<nodesList.count(); i++) {
            QDomElement indexElement = nodesList.at(i).toElement();

            if(indexElement == task) {
                m_rootElement.removeChild(indexElement);
                deleteTask(task);
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

void IndexTracker::flush()
{
    QFile file(m_trackerFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Can not open file:" << m_trackerFile;
        return;
    }

    QTextStream out(&file);
    out.setCodec("utf-8");

    out << m_doc.toString(4);
}

int IndexTracker::taskCount()
{
    return m_tasks.count();
}

IndexTaskIter* IndexTracker::getTaskIter()
{
    loadTask();

    IndexTaskIter *iter = new IndexTaskIter();

    foreach(IndexTask *task, m_tasks) {
        IndexTask *t = new IndexTask();
        TextBookReader *reader = new TextBookReader();
        t->bookID = task->bookID;
        t->task = task->task;
        t->reader = reader;

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