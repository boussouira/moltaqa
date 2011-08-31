#include "newbookwriter.h"
#include "bookexception.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "common.h"
#include "bookinfo.h"

#include <qsettings.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qdebug.h>

NewBookWriter::NewBookWriter()
{
    m_tempFolder = MainWindow::mainWindow()->libraryInfo()->tempDir();
    m_pageId = 0;
    m_threadID = 0;
    m_isTafessir = false;
}

QString NewBookWriter::bookPath()
{
    return m_bookPath;
}

void NewBookWriter::createNewBook(QString bookPath)
{
    // TODO: check if this file exsists
    if(bookPath.isEmpty())
        m_bookPath = Utils::genBookName(m_tempFolder, true);
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
    m_bookQuery.exec("DROP TABLE IF EXISTS bookPages");
    m_bookQuery.exec("DROP TABLE IF EXISTS bookIndex");
    m_bookQuery.exec("DROP TABLE IF EXISTS bookMeta");
    m_bookQuery.exec("DROP TABLE IF EXISTS tafessirMeta");


    m_bookQuery.exec("CREATE TABLE IF NOT EXISTS bookPages ("
                     "id INTEGER PRIMARY KEY, "
                     "pageNum INTEGER, "
                     "partNum INTEGER, "
                     "pageText BLOB)");

    // TODO: categorie order
    m_bookQuery.exec("CREATE TABLE IF NOT EXISTS bookIndex ("
                     "id INTEGER PRIMARY KEY,"
                     "pageID INTEGER,"
                     "parentID INTEGER,"
                     "title TEXT)");

    // This table will store book's comments and haddith number
    m_bookQuery.exec("CREATE TABLE IF NOT EXISTS bookMeta ("
                     "id INTEGER PRIMARY KEY,"
                     "page_id INTEGER,"
                     "haddit_number INTEGER,"
                     "comment TEXT)");

    m_bookQuery.exec("CREATE TABLE IF NOT EXISTS tafessirMeta ("
                     "page_id INTEGER UNIQUE,"
                     "aya_number INTEGER,"
                     "sora_number INTEGER)");
}

int NewBookWriter::addPage(const QString &text, int pageID, int pageNum, int partNum)
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
        LOG_SQL_ERROR(m_bookQuery);
    }

    return m_pageId;
}

int NewBookWriter::addPage(const QString &text, int pageID, int pageNum, int partNum, int ayaNum, int soraNum)
{
    int ret;
    ret = addPage(text, pageID, pageNum, partNum);

    m_bookQuery.prepare("INSERT INTO tafessirMeta (page_id, aya_number, sora_number) VALUES (?, ?, ?)");
    m_bookQuery.bindValue(0, ret);
    m_bookQuery.bindValue(1, ayaNum);
    m_bookQuery.bindValue(2, soraNum);

    if(!m_bookQuery.exec()){
        LOG_SQL_ERROR(m_bookQuery);
    }

    if(!m_isTafessir)
        m_isTafessir = true;

    return ret;
}

void NewBookWriter::addHaddithNumber(int page_id, int hno)
{
    if(hno == 0)
        return;

    m_bookQuery.prepare("INSERT INTO bookMeta (id, page_id, haddit_number) VALUES (NULL, ?, ?)");
    m_bookQuery.bindValue(0, page_id);
    m_bookQuery.bindValue(1, hno);

    if(!m_bookQuery.exec()){
        LOG_SQL_ERROR(m_bookQuery);
    }
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
        LOG_SQL_ERROR(m_bookQuery);
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
    if(!m_isTafessir)
        m_bookQuery.exec("DROP TABLE IF EXISTS tafessirMeta");

    // TODO: check if the commit success
    m_bookDB.commit();
    //qDebug("[*] Writting take %d ms", m_time.elapsed());
}

