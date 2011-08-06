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

    m_bookDB = QSqlDatabase::addDatabase("QSQLITE", "newBookDB");
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
}

void NewBookWriter::createFileInfo()
{
    // TODO: we should make this faster
    BookInfo bookInfo;
    m_bookQuery.exec("SELECT MAX(partNum), MIN(pageNum), MAX(pageNum), MIN(id), MAX(id) from bookPages ");
    if(m_bookQuery.next()) {
        bool ok;
        int parts = m_bookQuery.value(0).toInt(&ok);
        if(!ok)
            qWarning("Can't get max part number");

        bookInfo.setFirstID(m_bookQuery.value(3).toInt());
        bookInfo.setLastID(m_bookQuery.value(4).toInt());

        if(parts == 1) {
            bookInfo.setPartsCount(parts);
            bookInfo.setFirstPage(m_bookQuery.value(1).toInt());
            bookInfo.setLastPage(m_bookQuery.value(2).toInt());
        } else if(parts > 1) {
            bookInfo.setPartsCount(parts);
            for(int i=1;i<=parts;i++) {
                m_bookQuery.exec(QString("SELECT MIN(pageNum), MAX(pageNum) from bookPages WHERE partNum = %2 ").arg(i));
                if(m_bookQuery.next()) {
                    bookInfo.setFirstPage(m_bookQuery.value(0).toInt(), i);
                    bookInfo.setLastPage(m_bookQuery.value(1).toInt(), i);
                }
            }
        }
    }

    m_bookInfo = bookInfo.toString();
}

void NewBookWriter::endReading()
{
    // TODO: check if the commit success
    m_bookDB.commit();
    createFileInfo();
/*
    m_bookDB.transaction();
    updateIndexTable();
    m_bookDB.commit();
*/
}

QString NewBookWriter::serializeBookInfo()
{
    return m_bookInfo;
}

/*
void NewBookWriter::updateIndexTable()
{

//    int left=1;

//    m_bookQuery.exec("SELECT id FROM bookIndex WHERE lft = 0 ORDER BY id");
//    while(m_bookQuery.next()) {
//        qDebug("left: %d for %d", m_bookQuery.value(0).toInt(), left);
//        left = rebuildTrree(m_bookQuery.value(0).toInt(), left);
//    }

    rebuildTrree(0, 1);
}

int NewBookWriter::rebuildTrree(int parent, int left)
{
    int right = left+1;
    int id;

    QSqlQuery bookQuery(m_bookDB);
    bookQuery.prepare("SELECT id FROM bookIndex WHERE lft = ? ORDER BY id");
    bookQuery.bindValue(0, parent);

    if(bookQuery.exec()) {
        while (bookQuery.next()) {
            right = rebuildTrree(bookQuery.value(0).toInt(), right);
        }
    }

    bookQuery.prepare("UPDATE bookIndex SET lft = ?, rgt = ? WHERE id = ?");
    bookQuery.bindValue(0, left);
    bookQuery.bindValue(1, right);
    bookQuery.bindValue(2, parent);

    if(!bookQuery.exec())
        qDebug() << bookQuery.lastError();

    qDebug("left: %d Right: %d for %d", left, right, parent);

    return right+1;
}
*/
