#include "convertthread.h"
#include "importmodel.h"
#include "indexdb.h"
#include "newbookwriter.h"
#include "common.h"

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

ConvertThread::ConvertThread(QObject *parent) : QThread(parent)
{
    m_convertedFiles = 0;
}

void ConvertThread::run()
{
    QTime time;
    time.start();

    for(int i=0; i<m_files.count(); i++){
        try {
            QList<ImportModelNode*> nodesList;
            ImportFromShamelaBook(m_files.at(i), nodesList);

            foreach(ImportModelNode *node, nodesList)
                m_model->appendNode(node, QModelIndex());

            emit setProgress(i+1);
            m_convertedFiles++;

#ifdef USE_MDBTOOLS
            QSqlDatabase::removeDatabase("mdb");
            QSqlDatabase::removeDatabase("bok2sql");
#else
            QSqlDatabase::removeDatabase("mdb");
            QSqlDatabase::removeDatabase("ImportDB");
            QSqlDatabase::removeDatabase("exportDB");
#endif

        } catch(QString &what) {
            QMessageBox::critical(0,
                                  tr("خطأ عند الاستيراد"),
                                  what);
        }
    }

    m_convertTime = time.elapsed();
}

void ConvertThread::ImportFromShamelaBook(const QString &path, QList<ImportModelNode *> &nodes)
{
#ifdef  USE_MDBTOOLS
    MdbConverter mdb;
    QString dbPath = mdb.exportFromMdb(path);

    QSqlDatabase bookDB = QSqlDatabase::addDatabase("QSQLITE", "mdb");
    bookDB.setDatabaseName(dbPath);
#else
    QSqlDatabase bookDB = QSqlDatabase::addDatabase("QODBC", "mdb");
    bookDB.setDatabaseName(QString("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1").arg(path));
#endif

    if (!bookDB.open())
        throw tr("لا يمكن فتح قاعدة البيانات");

    QSqlQuery bookQuery(bookDB);

    bookQuery.exec("SELECT * FROM Main");
    while(bookQuery.next()) {
        int bkIdCol = bookQuery.record().indexOf("BkId");
        int bkCol = bookQuery.record().indexOf("bk");
        int authCol = bookQuery.record().indexOf("Auth");
        int catCol = bookQuery.record().indexOf("cat");
        int betakaCol = bookQuery.record().indexOf("Betaka");

        int bookID = bookQuery.value(bkIdCol).toInt();

        ImportModelNode *node = new ImportModelNode(BookInfo::NormalBook);
        node->setTypeName(getBookType(bookDB));
        node->setBookName(bookQuery.value(bkCol).toString());
        node->setAuthorName(bookQuery.value(authCol).toString());

        if(catCol != -1) { // Some old books doesn't have this column
            node->setCatName(bookQuery.value(catCol).toString()); // Must be set before CatID
            node->setCatID(m_indexDB->catIdFromName(bookQuery.value(catCol).toString()));
        } else {
            node->setCatID(-1);
        }

        if(betakaCol != -1) {
             node->setBookInfo(bookQuery.value(betakaCol).toString());
        }

        qDebug() << "Importing:" << node->bookName();

        copyBookFromShamelaBook(node, bookDB, bookID);
        nodes.append(node);

        QSqlDatabase::removeDatabase("newBookDB_0");
    }

    if(bookQuery.lastError().isValid())
        throw tr("حدث خطأ أثناء سحب المعلومات من قاعدة البيانات"
                     "<br><b style=\"direction:rtl\">%1</b>").arg(bookQuery.lastError().text());
}

void ConvertThread::copyBookFromShamelaBook(ImportModelNode *node, const QSqlDatabase &bookDB, int bookID)
{
    // TODO: update this code to handle tafassir an haddith number
    QSqlQuery query(bookDB);
    NewBookWriter writer;
    writer.createNewBook();
    writer.startReading();

    if(query.exec(QString("SELECT id, nass, page, part FROM b%1 ORDER BY id").arg(bookID))) {
        while(query.next()) {
            writer.addPage(query.value(1).toString(),
                           query.value(0).toInt(),
                           query.value(2).toInt(),
                           query.value(3).toInt());
        }
    } else {
        SQL_ERROR(query.lastError().text());
    }

    if(query.exec(QString("SELECT id, tit, lvl, sub FROM t%1 ORDER BY id").arg(bookID))) {
        while(query.next()) {
            writer.addTitle(query.value(1).toString(),
                           query.value(0).toInt(),
                           query.value(2).toInt());
        }
    } else {
       SQL_ERROR(query.lastError().text());
    }

    writer.endReading();

    node->setBookPath(writer.bookPath());
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
        throw tr("قاعدة البيانات المختار غير صحيحة")+"<br><b>"+
                tr("لم يتم العثور على جدول البيانات")+"</b>";

    query.exec(QString("SELECT * FROM %1").arg(bookTable));
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
