#include "convertthread.h"
#include "importmodel.h"
#include "librarymanager.h"
#include "newbookwriter.h"
#include "utils.h"
#include "bookexception.h"
#include "sqlutils.h"
#include "stringutils.h"
#include "booklistmanager.h"
#include "authorsmanager.h"
#include "booklistmanager.h"

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
    m_bookListManager = LibraryManager::instance()->bookListManager();
    m_authorsManager = LibraryManager::instance()->authorsManager();
}

void ConvertThread::run()
{
    QTime time;
    time.start();

    m_convertedFiles = 0;

    foreach(QString file, m_files){
        try {
            QFileInfo info(file);

            if(info.suffix().compare("bok", Qt::CaseInsensitive) == 0)
                ConvertShamelaBook(file);
            else
                qWarning() << "ConvertThread: File" << info.fileName() << "not handeled";

            emit setProgress(++m_convertedFiles);

#ifdef USE_MDBTOOLS
            if(m_tempDB.size()) {
                QFile::remove(m_tempDB);
                m_tempDB.clear();
            }
#endif
        } catch(BookException &e) {
             e.print();
        }
    }

    m_convertTime = time.elapsed();
}

void ConvertThread::ConvertShamelaBook(const QString &path)
{
    DatabaseRemover remover;

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
        ml_warn_db_error(bookDB);
        throw BookException(tr("لا يمكن فتح قاعدة البيانات"), path);
    }

    QSqlQuery bookQuery(bookDB);

    bookQuery.prepare("SELECT * FROM Main");
    ml_throw_on_query_exec_fail(bookQuery);

    int bkIdCol = bookQuery.record().indexOf("BkId");
    int bkCol = bookQuery.record().indexOf("bk");
    int authCol = bookQuery.record().indexOf("Auth");
    int catCol = bookQuery.record().indexOf("cat");
    int betakaCol = bookQuery.record().indexOf("Betaka");
    int infoCol = bookQuery.record().indexOf("inf");

    while(bookQuery.next()) {

        int bookID = bookQuery.value(bkIdCol).toInt();

        ImportModelNode *node = new ImportModelNode(LibraryBook::NormalBook);
        node->setTypeName(getBookType(bookDB));
        node->title = bookQuery.value(bkCol).toString().trimmed();
        node->authorName = bookQuery.value(authCol).toString().trimmed();

        if(catCol != -1) { // Some old books doesn't have this column
            CategorieInfo *foundCat = m_bookListManager->findCategorie(bookQuery.value(catCol).toString());
            if(foundCat)
                node->setCategories(foundCat->catID, foundCat->title);
            else
                node->setCategories(0);
        } else {
            node->setCategories(0);
        }

        if(betakaCol != -1)
             node->comment = bookQuery.value(betakaCol).toString();

        if(infoCol != -1)
            node->info = Utils::Html::format(bookQuery.value(infoCol).toString());

         AuthorInfoPtr foundAuth = m_authorsManager->findAuthor(bookQuery.value(authCol).toString(), false);
         if(foundAuth)
             node->setAuthor(foundAuth->id, foundAuth->name);
         else
             node->setAuthor(0, bookQuery.value(authCol).toString());

        copyBookFromShamelaBook(node, bookDB, bookID);
        m_model->appendNode(node);
    }

    remover.removeDatabase("mdb");
}

void ConvertThread::copyBookFromShamelaBook(ImportModelNode *node, const QSqlDatabase &bookDB, int bookID)
{
    QSqlQuery query(bookDB);

#ifdef USE_MDBTOOLS
    query.prepare(QString("SELECT * FROM b%1 LIMIT 1").arg(bookID));
#else
    query.prepare(QString("SELECT TOP 1 * FROM b%1").arg(bookID));
#endif

    ml_throw_on_query_exec_fail(query);

    int hnoCol = query.record().indexOf("hno");
    int ayaCol = query.record().indexOf("aya");
    int soraCol = query.record().indexOf("sora");

    NewBookWriter writer;
    writer.createNewBook();
    writer.startReading();

    if(ayaCol != -1 && soraCol != -1) {
        // This is a tafessir book
        if(hnoCol!=-1) {
            // We have hno column
            query.prepare(QString("SELECT id, nass, page, part, aya, sora, hno FROM b%1 ORDER BY id").arg(bookID));
            ml_throw_on_query_exec_fail(query);

            while(query.next()) {
                writer.addPage(query.value(1).toString(),
                               query.value(0).toInt(),
                               query.value(2).toInt(),
                               query.value(3).toInt(),
                               query.value(6).toInt(),
                               query.value(4).toInt(),
                               query.value(5).toInt());
            }
        } else {
            // We don't have hno column
            query.prepare(QString("SELECT id, nass, page, part, aya, sora FROM b%1 ORDER BY id").arg(bookID));
            ml_throw_on_query_exec_fail(query);

            while(query.next()) {
                writer.addPage(query.value(1).toString(),
                               query.value(0).toInt(),
                               query.value(2).toInt(),
                               query.value(3).toInt(),
                               -1,
                               query.value(4).toInt(),
                               query.value(5).toInt());
            }
        }
    } else {
        // This is a simple book
        if(hnoCol!=-1) {
            // We have hno column
            query.prepare(QString("SELECT id, nass, page, part, hno FROM b%1 ORDER BY id").arg(bookID));
            ml_throw_on_query_exec_fail(query);

            while(query.next()) {
                writer.addPage(query.value(1).toString(),
                               query.value(0).toInt(),
                               query.value(2).toInt(),
                               query.value(3).toInt(),
                               query.value(4).toInt());
            }
        } else {
            // We don't have hno column
            query.prepare(QString("SELECT id, nass, page, part FROM b%1 ORDER BY id").arg(bookID));
            ml_throw_on_query_exec_fail(query);

            while(query.next()) {
                writer.addPage(query.value(1).toString(),
                               query.value(0).toInt(),
                               query.value(2).toInt(),
                               query.value(3).toInt());
            }
        }
    }

    query.prepare(QString("SELECT id, tit, lvl, sub FROM t%1 ORDER BY id, sub").arg(bookID));
    ml_throw_on_query_exec_fail(query);

    while(query.next()) {
        writer.addTitle(query.value(1).toString(),
                        query.value(0).toInt(),
                        query.value(2).toInt());
    }

    writer.endReading();

    node->path = writer.bookPath();
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

    query.prepare(QString("SELECT * FROM %1").arg(bookTable));
    ml_throw_on_query_exec_fail(query);

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
