#include "librarycreator.h"
#include "common.h"
#include "newbookwriter.h"
#include "bookinfo.h"
#include <qdebug.h>
#include <qvariant.h>
#include <qfileinfo.h>

LibraryCreator::LibraryCreator()
{
    m_prevArchive = -1;
}

void LibraryCreator::openDB()
{
    m_bookDB = QSqlDatabase::addDatabase("QSQLITE", "newBookIndexDB");
    m_bookDB.setDatabaseName(m_library->booksIndexPath());

    if(!m_bookDB.open()) {
        qDebug() << QObject::tr("لم يمكن فتح قاعدة البيانات: %1").arg(m_library->booksIndexPath());
    }

    m_bookQuery = QSqlQuery(m_bookDB);
}

void LibraryCreator::createTables()
{
    m_bookQuery.exec("DROP TABLE IF EXISTS booksList");
    m_bookQuery.exec("DROP TABLE IF EXISTS catList");
    m_bookQuery.exec("DROP TABLE IF EXISTS bookMeta");
    m_bookQuery.exec("DROP TABLE IF EXISTS authorsList");

    m_bookQuery.exec("CREATE TABLE booksList ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                     "bookID INTEGER , "
                     "bookType INTEGER , "
                     "bookFlags INTEGER , "
                     "bookCat INTEGER , "
                     "bookName TEXT , "
                     "bookInfo TEXT , "
                     "bookEdition TEXT , "
                     "authorName TEXT , "
                     "authorID INTEGER , "
                     "fileName TEXT , "
                     "bookFolder TEXT, "
                     "pdfID INTEGER)");

    m_bookQuery.exec("CREATE TABLE catList ("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                     "title TEXT , "
                     "description TEXT , "
                     "catOrder INTEGER , "
                     "parentID INTEGER)");

    m_bookQuery.exec("CREATE TABLE bookMeta("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                     "book_info TEXT, "
                     "add_time INTEGER, "
                     "update_time INTEGER)");

    m_bookQuery.exec("CREATE TABLE authorsList("
                     "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                     "name TEXT, "
                     "full_name TEXT, "
                     "die_year INTEGER, "
                     "info BLOB)");
}

void LibraryCreator::addCat(CategorieInfo *cat)
{
    int lastId;

    m_bookQuery.prepare("INSERT INTO catList (id, title, parentID, catOrder) VALUES (NULL, ?, ?, ?)");
    m_bookQuery.bindValue(0, cat->name);
    m_bookQuery.bindValue(1, m_levels.value(cat->level-1, 0));
    m_bookQuery.bindValue(2, 0);

    if(m_bookQuery.exec()) {
        lastId = m_bookQuery.lastInsertId().toInt();
        m_catMap.insert(cat->id, lastId);
        //qDebug("Cat %d -> %d", cat->id, lastId);
    } else {
        SQL_ERROR(m_bookQuery.lastError().text());
    }

    int levelParent = m_levels.value(cat->level, -1);
    if(levelParent == -1) {
        m_levels.insert(cat->level, lastId);
    } else {
            m_levels[cat->level] = lastId;
    }
}

void LibraryCreator::addAuthor(AuthorInfo *auth)
{
    m_bookQuery.prepare("INSERT INTO authorsList (id, name, full_name, die_year, info) VALUES (NULL, ?, ?, ?, ?)");
    m_bookQuery.bindValue(0, auth->name);
    m_bookQuery.bindValue(1, auth->fullName);
    m_bookQuery.bindValue(2, auth->dieYear);
    m_bookQuery.bindValue(3, qCompress(auth->info.toLocal8Bit()));

    if(!m_bookQuery.exec())
        SQL_ERROR(m_bookQuery.lastError().text());
}

void LibraryCreator::addBook(ShamelaBookInfo *book)
{
    QString connName(QString("mdb_%1").arg(book->archive));
    QString path = genBookName(m_library->booksDir(), true);

    {
        NewBookWriter bookWrite;
        bookWrite.createNewBook(path);

        QSqlDatabase bookDB;
        if(book->archive && book->archive == m_prevArchive) {
            bookDB = QSqlDatabase::database(connName);
        } else {
            // Remove old connection
            qDebug("Remove connection: %d", m_prevArchive);
            QString prevConnName(QString("mdb_%1").arg(m_prevArchive));
            QSqlDatabase::database(prevConnName, false).close();
            QSqlDatabase::removeDatabase(prevConnName);

            bookDB = QSqlDatabase::addDatabase("QODBC", connName);
            bookDB.setDatabaseName(QString("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1").arg(book->path));

            m_prevArchive = book->archive;
        }

        if(!bookDB.isOpen()) {
            if (!bookDB.open()) {
                DB_OPEN_ERROR(book->path);
            }
        }

        QSqlQuery query(bookDB);
        bookWrite.startReading();

        query.prepare(QString("SELECT id, nass, page, part FROM %1 ORDER BY id").arg(book->mainTable));
        if(query.exec()) {
            while(query.next()) {
                bookWrite.addPage(query.value(1).toString(),
                                  query.value(0).toInt(),
                                  query.value(2).toInt(),
                                  query.value(3).toInt());
            }
        } else {
            SQL_ERROR(query.lastError().text());
        }

        query.prepare(QString("SELECT id, tit, lvl, sub FROM %1 ORDER BY id").arg(book->tocTable));
        if(query.exec()) {
            while(query.next()) {
                bookWrite.addTitle(query.value(1).toString(),
                                   query.value(0).toInt(),
                                   query.value(2).toInt());
            }
        } else {
            SQL_ERROR(query.lastError().text());
        }

        bookWrite.endReading();
    }

    QSqlDatabase::removeDatabase("newBookDB");

    if(!book->archive)
        QSqlDatabase::removeDatabase(connName);

    importBook(book, path);
}

void LibraryCreator::start()
{
    m_bookDB.transaction();
}

void LibraryCreator::done()
{
    if(m_bookDB.commit())
        qDebug("Commit...");
    else
        qDebug("Error when committing");
}

void LibraryCreator::importBook(ShamelaBookInfo *book, QString path)
{
    QFileInfo fileInfo(path);

    m_bookQuery.prepare("INSERT INTO booksList (id, bookID, bookType, bookFlags, bookCat,"
                       "bookName, bookInfo, authorName, authorID, fileName, bookFolder)"
                       "VALUES(NULL, :book_id, :book_type, :book_flags, :cat_id, :book_name, "
                       ":book_info, :author_name, :author_id, :file_name, :book_folder)");

    m_bookQuery.bindValue(":book_id", 0);
    m_bookQuery.bindValue(":book_type", BookInfo::NormalBook);
    m_bookQuery.bindValue(":book_flags", 0);
    m_bookQuery.bindValue(":cat_id", m_catMap.value(book->cat, 0));
    m_bookQuery.bindValue(":book_name", book->name);
    m_bookQuery.bindValue(":book_info", book->info);
    m_bookQuery.bindValue(":author_name", book->authName);
    m_bookQuery.bindValue(":author_id", book->authorID);
    m_bookQuery.bindValue(":file_name", fileInfo.fileName()); // Add file name
    m_bookQuery.bindValue(":book_folder", QVariant(QVariant::String));

    if(!m_bookQuery.exec()) {
        SQL_ERROR(m_bookQuery.lastError().text());
    }
}
