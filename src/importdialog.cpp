#include "importdialog.h"
#include "ui_importdialog.h"
#include "importmodel.h"
#include "importdelegates.h"
#ifdef Q_OS_LINUX
#include "mdbconverter.h"
#endif

#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qstandarditemmodel.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qsqlrecord.h>
#include <qsqlerror.h>
#include <qdebug.h>

ImportDialog::ImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportDialog)
{
    ui->setupUi(this);
    m_model = new ImportModel(ui->treeView);

    ImportModelNode *node = new ImportModelNode(BookInfo::NormalBook);
    m_model->setRootNode(node);

    ui->treeView->setItemDelegateForColumn(2, new BookTypeDelegate(ui->treeView));
    ui->treeView->setItemDelegateForColumn(3, new CategorieDelegate(ui->treeView));
    ui->treeView->setModel(m_model);
}

ImportDialog::~ImportDialog()
{
    delete ui;
}

void ImportDialog::on_pushCancel_clicked()
{
    reject();
}

void ImportDialog::on_pushAdd_clicked()
{
    QString bokPath = selectShamelBook();
    if(bokPath.isEmpty())
        return;

    try {
        ImportModelNode *node = getBookInfo(bokPath);
        m_model->appendNode(node, QModelIndex());
    } catch(QString &what) {
        QMessageBox::critical(this, trUtf8("خطأ عند الاستيراد"),
                              what);
    }

    QSqlDatabase::removeDatabase("ImportBookInfo");
}

QString ImportDialog::selectShamelBook()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Shamela book (*.bok);; All files (*.*)"));
    dialog.setViewMode(QFileDialog::Detail);

    if (dialog.exec())
        return dialog.selectedFiles().first();
    else
        return QString();
}

void ImportDialog::on_pushDelete_clicked()
{
    QModelIndexList indexes = ui->treeView->selectionModel()->selectedRows();
    for(int i=0; i<indexes.count(); i++) {
        m_model->removeRow(indexes.at(i).row(), indexes.at(i).parent());
    }
}

void ImportDialog::on_pushImport_clicked()
{
    ImportModelNode *rootNode = m_model->nodeFromIndex(QModelIndex());
    QSqlDatabase indexDB = QSqlDatabase::addDatabase("QSQLITE", "BooksInfoDB");
    indexDB.setDatabaseName(QString("%1/books/books_index.db")
                            .arg("/home/naruto/Programming/alkotobiya"));
    if(!indexDB.open())
        qDebug("[%s:%d] Cannot open database.", __FILE__, __LINE__);
    QSqlQuery *indexQuery = new QSqlQuery(indexDB);

    foreach(ImportModelNode *node, rootNode->childrenList()) {
        QString qurey = QString("INSERT INTO booksList (id, bookID, bookType, bookFlags, bookCat,"
                                "bookName, bookInfo, authorName, authorID, fileName, bookFolder)"
                                "VALUES(NULL, 0, %1, %2, %3, '%4', '%5', '%6', %7, '%8', '')")
                .arg(node->getNodeType())
                .arg(0)
                .arg(node->getCatID())
                .arg(node->getBookName())
                .arg(node->getInfoToolTip())
                .arg(node->getAuthorName())
                .arg(0)
                .arg(node->getBookPath().split("/").last());
        QFile::copy(node->getBookPath(), QString("%1/books/%2")
                    .arg("/home/naruto/Programming/alkotobiya")
                    .arg(node->getBookPath().split("/").last()));
         if(indexQuery->exec(qurey))
            qDebug() << "[+]" << node->getBookName();
         else {
             qDebug() << "Error:" << indexQuery->lastError().text();
             qDebug() << "Query:" << qurey ;
         }
    }
}

ImportModelNode *ImportDialog::getBookInfo(const QString &path)
{
    QString dbPath = path;

#ifdef Q_OS_WIN32
    QSqlDatabase m_bookDB = QSqlDatabase::addDatabase("QODBC", "mdb");
    QString mdbpath = QString("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1")
                      .arg(dbPath);
    m_bookDB.setDatabaseName(mdbpath);

    if (!m_bookDB.open())
        throw trUtf8("لا يمكن فتح قاعدة البيانات");

#else
    dbPath.append(".sqlite");
    if(QFile::exists(dbPath)) {
        QFile fi(dbPath);
        fi.remove(dbPath);
    }
    MdbConverter mdb;
    mdb.exportFromMdb(path, dbPath);
    QSqlDatabase m_bookDB = QSqlDatabase::addDatabase("QSQLITE", "mdb");
    m_bookDB.setDatabaseName(dbPath);

#endif

    if (!m_bookDB.open())
        throw trUtf8("لا يمكن فتح قاعدة البيانات");

    QSqlQuery *m_bookQuery = new QSqlQuery(m_bookDB);

    if(!m_bookDB.tables().contains("Main", Qt::CaseInsensitive))
        throw trUtf8("قاعدة البيانات المختار غير صحيحة")+"<br><b>"+
                trUtf8("لم يتم العثور على جدول البيانات الرئيسي")+"</b>";

    m_bookQuery->exec("SELECT bk, Auth, cat FROM Main");

    if(m_bookQuery->next()) {
        ImportModelNode *node = new ImportModelNode(BookInfo::NormalBook);
        node->setTypeName(getBookType(m_bookDB));
        node->setBookName(m_bookQuery->value(0).toString());
        node->setAuthorName(m_bookQuery->value(1).toString());
        node->setCatName(!m_bookQuery->value(2).toString().isEmpty() ?
                         m_bookQuery->value(2).toString() :
                         trUtf8("-- غير محدد --"));
        node->setBookPath(dbPath);
        return node;
    } else
        throw trUtf8("حدث خطأ أثناء سحب المعلومات من قاعدة البيانات");

}

QString ImportDialog::getBookType(const QSqlDatabase &bookDB)
{
    QSqlQuery *query = new QSqlQuery(bookDB);
    QSqlQuery *hnoQuery = new QSqlQuery(bookDB);
    QString bookTable;

    foreach(QString ta, bookDB.tables()) {
        if( ta.contains(QRegExp("(b[0-9]+|book)")) )
            bookTable = ta;
    }
    if(bookTable.isEmpty())
        throw trUtf8("قاعدة البيانات المختار غير صحيحة")+"<br><b>"+
                trUtf8("لم يتم العثور على جدول البيانات")+"</b>";

    query->exec(QString("SELECT * FROM %1").arg(bookTable));
    if(query->next()) {
        int hno = query->record().indexOf("hno");
        int aya = query->record().indexOf("aya");
        int sora = query->record().indexOf("sora");

        if (hno != -1 && aya == -1  && sora == -1){
            hnoQuery->exec(QString("SELECT MAX(hno) FROM %1").arg(bookTable));
            if(hnoQuery->next()){
                if(!hnoQuery->value(0).toString().isEmpty())
                    return trUtf8("متن حديث");
                else
                    return trUtf8("عادي");
            }
        } else if(aya != -1 && sora != -1)
            return trUtf8("تفسير");
        else
            return trUtf8("عادي");
    }
}
