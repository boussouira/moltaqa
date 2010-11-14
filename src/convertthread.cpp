#include "convertthread.h"
#include "importmodel.h"
#include "mdbconverter.h"
#include "booksindexdb.h"

#include "qmessagebox.h"
#include "qsqldatabase.h"
#include <qsqlquery.h>
#include <qsqlrecord.h>
#include <qsqlerror.h>
#include <qdatetime.h>

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
            getBookInfo(m_files.at(i), nodesList);

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
                                  trUtf8("خطأ عند الاستيراد"),
                                  what);
        }
    }

    m_convertTime = time.elapsed();
}

void ConvertThread::getBookInfo(const QString &path, QList<ImportModelNode *> &nodes)
{
    MdbConverter mdb;
    QString dbPath = mdb.exportFromMdb(path);

    QSqlDatabase m_bookDB = QSqlDatabase::addDatabase("QSQLITE", "mdb");
    m_bookDB.setDatabaseName(dbPath);

    if (!m_bookDB.open())
        throw trUtf8("لا يمكن فتح قاعدة البيانات");

    QSqlQuery bookQuery(m_bookDB);

    bookQuery.exec("SELECT * FROM Main");
    while(bookQuery.next()) {
        int bkCol = bookQuery.record().indexOf("bk");
        int authCol = bookQuery.record().indexOf("Auth");
        int catCol = bookQuery.record().indexOf("cat");

        ImportModelNode *node = new ImportModelNode(BookInfo::NormalBook);
        node->setTypeName(getBookType(m_bookDB));
        node->setBookName(bookQuery.value(bkCol).toString());
        node->setAuthorName(bookQuery.value(authCol).toString());

        if(catCol != -1) { // Some old books doesn't have this column
            node->setCatName(bookQuery.value(catCol).toString()); // Must be set before CatID
            node->setCatID(m_indexDB->getCatIdFromName(bookQuery.value(catCol).toString()));
        } else
             node->setCatID(-1);

        node->setBookPath(dbPath);
        nodes.append(node);
    }

    if(bookQuery.lastError().isValid())
        throw trUtf8("حدث خطأ أثناء سحب المعلومات من قاعدة البيانات"
                     "<br><b style=\"direction:rtl\">%1</b>").arg(bookQuery.lastError().text());
}

QString ConvertThread::getBookType(const QSqlDatabase &bookDB)
{
    QSqlQuery query(bookDB);
    QSqlQuery hnoQuery(bookDB);
    QString bookTable;

    foreach(QString ta, bookDB.tables()) {
        if( ta.contains(QRegExp("(b[0-9]+|book)")) )
            bookTable = ta;
    }
    if(bookTable.isEmpty())
        throw trUtf8("قاعدة البيانات المختار غير صحيحة")+"<br><b>"+
                trUtf8("لم يتم العثور على جدول البيانات")+"</b>";

    query.exec(QString("SELECT * FROM %1").arg(bookTable));
    if(query.next()) {
        int hno = query.record().indexOf("hno");
        int aya = query.record().indexOf("aya");
        int sora = query.record().indexOf("sora");

        if (hno != -1 && aya == -1  && sora == -1){
            hnoQuery.exec(QString("SELECT MAX(hno) FROM %1").arg(bookTable));
            if(hnoQuery.next()){
                if(!hnoQuery.value(0).toString().isEmpty())
                    return trUtf8("متن حديث");
                else
                    return trUtf8("عادي");
            }
        } else if(aya != -1 && sora != -1)
            return trUtf8("تفسير");
    }

    return trUtf8("عادي");
}
