#include "newbookwriter.h"
#include "bookexception.h"
#include <qsettings.h>
#include <qdir.h>
#include <QDateTime>
#include <QDebug>
#include <bookinfo.h>

NewBookWriter::NewBookWriter()
{
    QSettings settings;
    QDir dir(settings.value("library_dir").toString());
    if(!dir.exists("temp"))
        dir.mkdir("temp");

    m_tempFolder = dir.filePath("temp");
    m_pageId = 0;
    m_threadID = 0;
}

QString NewBookWriter::bookPath()
{
    return m_bookPath;
}

void NewBookWriter::createNewBook(QString bookPath)
{
    // TODO: check if this file exsists
    if(bookPath.isEmpty())
        m_bookPath = QString("%1/book_%2.sqlite").arg(m_tempFolder).arg(QDateTime::currentDateTime().toMSecsSinceEpoch());
    else
        m_bookPath = bookPath;

    m_bookDB = QSqlDatabase::addDatabase("QSQLITE", QString("newBookDB_%1").arg(m_threadID));
    m_bookDB.setDatabaseName(m_bookPath);
    if(!m_bookDB.open()) {
        throw BookException(QObject::tr("لم يمكن فتح قاعدة البيانات"), m_bookPath);
    }

    m_bookQuery = QSqlQuery(m_bookDB);

    createBookTables();
}

void NewBookWriter::createBookTables()
{
    // TODO: the id columun should be AUTO INCEREMENT?
    m_bookQuery.exec("DROP TABLE IF EXISTS bookPages");
    m_bookQuery.exec("CREATE TABLE IF NOT EXISTS bookPages ("
                     "id INTEGER PRIMARY KEY,"
                     "pageText BLOB,"
                     "pageNum INTEGER,"
                     "partNum INTEGER)");

    // TODO: categorie order
    m_bookQuery.exec("DROP TABLE IF EXISTS bookIndex");
    m_bookQuery.exec("CREATE TABLE IF NOT EXISTS bookIndex ("
                     "id INTEGER PRIMARY KEY,"
                     "pageID INTEGER,"
                     "parentID INTEGER,"
                     "title TEXT)");
}

void NewBookWriter::addPage(const QString &text, int pageID, int pageNum, int partNum)
{
    if(partNum<1)
        partNum = 1;

    if(pageNum<1)
        pageNum = 1;

    m_bookQuery.prepare("INSERT INTO bookPages (id, pageText, pageNum, partNum) VALUES (NULL, ?, ?, ?)");
    m_bookQuery.bindValue(0, qCompress(text.toUtf8()));
    m_bookQuery.bindValue(1, pageNum);
    m_bookQuery.bindValue(2, partNum);

    if(m_bookQuery.exec()){
        m_pageId++; // Last inserted id
        m_idsHash.insert(pageID, m_pageId);
    } else {
        // TODO: throw an exception
        qDebug() << "Error 123:" <<m_bookQuery.lastError().text();
    }

    /*
    if(m_firstPage.value(partNum, -1) == -1)
        m_firstPage.insert(partNum, pageNum);

    int lastPage = m_lastPage.value(partNum, -1);
    if(lastPage < pageNum || lastPage == -1)
        m_lastPage.insert(partNum, pageNum);
    */
}

void NewBookWriter::addTitle(const QString &title, int tid, int level)
{
    int id = m_idsHash.value(tid, -1);
    if(id == -1)
        id = m_prevID;
    else
        m_prevID = id;

    m_bookQuery.prepare("INSERT INTO bookIndex (id, pageID, parentID, title) VALUES (NULL, ?, ?, ?)");
    m_bookQuery.bindValue(0, id);
    m_bookQuery.bindValue(1, m_levels.value(level-1, 0));
    m_bookQuery.bindValue(2, title);

    if(m_bookQuery.exec()){
        m_titleID++;
    } else {
        // TODO: throw an exception
        qDebug() << "Error 123:" <<m_bookQuery.lastError().text();
    }

    int levelParent = m_levels.value(level, -1);
    if(levelParent == -1) {
        m_levels.insert(level, m_titleID);
    } else {
            m_levels[level] = m_titleID;
    }
    m_lastLevel = level;
}

void NewBookWriter::startReading()
{
    m_bookDB.transaction();
    m_pageId = 0;
    m_prevID = 1;
    m_lastLevel = -1;
    m_titleID = 0;

    //m_time.start();
}

void NewBookWriter::endReading()
{
    // TODO: check if the commit success
    m_bookDB.commit();
    //qDebug("[*] Writting take %d ms", m_time.elapsed());
}

