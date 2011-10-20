#include "convertthread.h"
#include "importmodel.h"
#include "librarymanager.h"
#include "newbookwriter.h"
#include "utils.h"
#include "bookexception.h"

#ifdef USE_MDBTOOLS
#include "mdbconverter.h"
#endif

#include <qmessagebox.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlrecord.h>
#include <qsqlerror.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <qfileinfo.h>

ConvertThread::ConvertThread(QObject *parent) : QThread(parent)
{
    m_convertedFiles = 0;
}

void ConvertThread::run()
{
    QTime time;
    time.start();

    m_convertedFiles = 0;

    qsrand(QDateTime::currentDateTime().toTime_t());

    foreach(QString file, m_files){
        try {
            QFileInfo info(file);

            if(info.suffix().compare("bok", Qt::CaseInsensitive) == 0)
                ConvertShamelaBook(file);
            else
                qWarning() << "File" << info.fileName() << "not handeled";

            emit setProgress(++m_convertedFiles);

#ifdef USE_MDBTOOLS
            QSqlDatabase::removeDatabase("bok2sql");

            if(!m_tempDB.isEmpty()) {
                QFile::remove(m_tempDB);
                m_tempDB.clear();
            }
#else

#endif

        } catch(BookException &e) {
            QMessageBox::critical(0,
                                  tr("خطأ عند الاستيراد"),
                                  e.what());
        }
    }

    m_convertTime = time.elapsed();
}

void ConvertThread::ConvertShamelaBook(const QString &path)
{
#ifdef USE_MDBTOOLS
    MdbConverter mdb;
    m_tempDB = mdb.exportFromMdb(path);

    QSqlDatabase bookDB = QSqlDatabase::addDatabase("QSQLITE", "mdb");
    bookDB.setDatabaseName(m_tempDB);
#else
    QSqlDatabase bookDB = QSqlDatabase::addDatabase("QODBC", "mdb");
    bookDB.setDatabaseName(QString("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1").arg(path));
#endif

    if (!bookDB.open()) {
        LOG_DB_ERROR(bookDB);
        throw BookException(tr("لا يمكن فتح قاعدة البيانات"), path);
    }

    QSqlQuery bookQuery(bookDB);

    if(!bookQuery.exec("SELECT * FROM Main")) {
        LOG_SQL_ERROR(bookQuery);
        throw BookException(tr("حدث خطأ أثناء سحب المعلومات من قاعدة البيانات"
                     "<br><b style=\"direction:rtl\">%1</b>").arg(bookQuery.lastError().text()), path);
    }

    while(bookQuery.next()) {
        int bkIdCol = bookQuery.record().indexOf("BkId");
        int bkCol = bookQuery.record().indexOf("bk");
        int authCol = bookQuery.record().indexOf("Auth");
        int catCol = bookQuery.record().indexOf("cat");
        int betakaCol = bookQuery.record().indexOf("Betaka");

        int bookID = bookQuery.value(bkIdCol).toInt();

        ImportModelNode *node = new ImportModelNode(LibraryBook::NormalBook);
        node->typeName = getBookType(bookDB);
        node->bookName = bookQuery.value(bkCol).toString();
        node->authorName = bookQuery.value(authCol).toString();

        if(catCol != -1) { // Some old books doesn't have this column
            QPair<int, QString> foundCat = m_libraryManager->findCategorie(bookQuery.value(catCol).toString());
            node->setCategories(foundCat.first, foundCat.second);
        } else {
            node->setCategories(0);
        }

        if(betakaCol != -1) {
             node->bookInfo = bookQuery.value(betakaCol).toString();
        }

         QPair<int, QString> foundAuth = m_libraryManager->findAuthor(bookQuery.value(authCol).toString());
         if(foundAuth.first)
             node->setAuthor(foundAuth.first, foundAuth.second);
         else
             node->setAuthor(foundAuth.first, bookQuery.value(authCol).toString());


        qDebug() << "Importing:" << node->bookName;

        copyBookFromShamelaBook(node, bookDB, bookID);
        m_model->appendNode(node);

        QSqlDatabase::removeDatabase(QString("newBookDB_%1").arg((int)currentThreadId()));
    }

    bookDB = QSqlDatabase();
    bookQuery.clear();
    QSqlDatabase::removeDatabase("mdb");
}

void ConvertThread::copyBookFromShamelaBook(ImportModelNode *node, const QSqlDatabase &bookDB, int bookID)
{
    QSqlQuery query(bookDB);

#ifdef USE_MDBTOOLS
    if(!query.exec(QString("SELECT * FROM b%1 LIMIT 1").arg(bookID)))
        LOG_SQL_ERROR(query);
#else
    if(!query.exec(QString("SELECT TOP 1 * FROM b%1").arg(bookID)))
        LOG_SQL_ERROR(query);
#endif

    int hnoCol = query.record().indexOf("hno");
    int ayaCol = query.record().indexOf("aya");
    int soraCol = query.record().indexOf("sora");

    NewBookWriter writer;
    writer.setThreadID((int)currentThreadId());
    writer.createNewBook();
    writer.startReading();

    int lastID=0;

    if(ayaCol != -1 && soraCol != -1) {
        // This is a tafessir book
        if(hnoCol!=-1) {
            // We have hno column
            if(query.exec(QString("SELECT id, nass, page, part, aya, sora, hno FROM b%1 ORDER BY id").arg(bookID))) {
                while(query.next()) {
                    lastID = writer.addPage(query.value(1).toString(),
                                            query.value(0).toInt(),
                                            query.value(2).toInt(),
                                            query.value(3).toInt(),
                                            query.value(4).toInt(),
                                            query.value(5).toInt());
                    writer.addHaddithNumber(lastID, query.value(6).toInt());
                }
            } else {
                LOG_SQL_ERROR(query);
            }
        } else {
            // We don't have hno column
            if(query.exec(QString("SELECT id, nass, page, part, aya, sora FROM b%1 ORDER BY id").arg(bookID))) {
                while(query.next()) {
                    writer.addPage(query.value(1).toString(),
                                   query.value(0).toInt(),
                                   query.value(2).toInt(),
                                   query.value(3).toInt(),
                                   query.value(4).toInt(),
                                   query.value(5).toInt());
                }
            } else {
                LOG_SQL_ERROR(query);
            }
        }
    } else {
        // This is a simple book
        if(hnoCol!=-1) {
            // We have hno column
            if(query.exec(QString("SELECT id, nass, page, part, hno FROM b%1 ORDER BY id").arg(bookID))) {
                while(query.next()) {
                    lastID = writer.addPage(query.value(1).toString(),
                                            query.value(0).toInt(),
                                            query.value(2).toInt(),
                                            query.value(3).toInt());
                    writer.addHaddithNumber(lastID, query.value(4).toInt());
                }
            } else {
                LOG_SQL_ERROR(query);
            }
        } else {
            // We don't have hno column
            if(query.exec(QString("SELECT id, nass, page, part FROM b%1 ORDER BY id").arg(bookID))) {
                while(query.next()) {
                    lastID = writer.addPage(query.value(1).toString(),
                                            query.value(0).toInt(),
                                            query.value(2).toInt(),
                                            query.value(3).toInt());
                }
            } else {
                LOG_SQL_ERROR(query);
            }
        }
    }

    if(query.exec(QString("SELECT id, tit, lvl, sub FROM t%1 ORDER BY id").arg(bookID))) {
        while(query.next()) {
            writer.addTitle(query.value(1).toString(),
                            query.value(0).toInt(),
                            query.value(2).toInt());
        }
    } else {
        LOG_SQL_ERROR(query);
    }

    writer.endReading();

    node->bookPath = writer.bookPath();
}

QString ConvertThread::getBookType(const QSqlDatabase &bookDB)
{
    QSqlQuery query(bookDB);
    QString bookTable;

    foreach(QString ta, bookDB.tables()) {
        if( ta.contains(QRegExp("(b[0-9]+|book)")) )
            bookTable = ta;
    }

    if(bookTable.isEmpty())
        throw BookException(tr("قاعدة البيانات المختار غير صحيحة، "
                               "لم يتم العثور على جدول البيانات"));

    if(!query.exec(QString("SELECT * FROM %1").arg(bookTable))) {
        LOG_SQL_ERROR(query);
    }

    if(query.next()) {
        //int hno = query.record().indexOf("hno");
        int aya = query.record().indexOf("aya");
        int sora = query.record().indexOf("sora");

        if(aya != -1 && sora != -1)
            return tr("تفسير");
        else
            return tr("عادي");
    }

    return tr("عادي");
}
