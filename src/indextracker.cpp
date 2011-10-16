#include "indextracker.h"
#include "mainwindow.h"
#include "bookexception.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qdebug.h>

bool operator ==(const QDomElement &node, const IndexTask &task)
{
    return (node.attribute("book").toInt() == task.bookID &&
            IndexTask::stringToTask(node.attribute("type")) == task.task);
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
        IndexTask task;
        task.bookID = taskNode.attribute("book").toInt();
        task.task = IndexTask::stringToTask(taskNode.attribute("type"));
        m_tasks.append(task);

        taskNode = taskNode.nextSiblingElement("task");
    }
}

void IndexTracker::addTask(IndexTask task)
{
    if(!m_tasks.contains(task)) {

        QDomElement newTask = m_doc.createElement("task");

        newTask.setAttribute("book", task.bookID);
        newTask.setAttribute("type", IndexTask::taskToString(task.task));
        m_rootElement.appendChild(newTask);

        m_tasks.append(task);
        flush();
    } else {
        qWarning() << "Task exists:" << task;
    }
}

void IndexTracker::removeTask(IndexTask task)
{
    QDomNodeList nodesList = m_rootElement.elementsByTagName("task");

    for (int i=0; i<nodesList.count(); i++) {
        QDomElement indexElement = nodesList.at(i).toElement();

        if(indexElement == task) {
            m_rootElement.removeChild(indexElement);

            loadTask();
            break;
        }
    }
}

void IndexTracker::flush()
{
    QFile file(m_trackerFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning("File not opened");
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
    IndexTaskIter *iter = new IndexTaskIter();

    foreach(IndexTask task, m_tasks) {
        IndexTask *t = new IndexTask();
        t->bookID = task.bookID;
        t->task = task.task;
        t->book = m_libraryManager->getBookInfo(t->bookID);
        iter->addTask(t);
    }

    return iter;
}
