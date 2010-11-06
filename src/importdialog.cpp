#include "importdialog.h"
#include "ui_importdialog.h"
#include "importmodel.h"
#include "importdelegates.h"
#include "booksindexdb.h"
#include "mdbconverter.h"

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
    m_indexDB = new BooksIndexDB;

    ImportModelNode *node = new ImportModelNode(BookInfo::NormalBook);
    m_model->setRootNode(node);

    ui->treeView->setItemDelegateForColumn(2, new BookTypeDelegate(ui->treeView));
    ui->treeView->setItemDelegateForColumn(3,
                                           new CategorieDelegate(ui->treeView,
                                                                 m_indexDB->getListModel(false)));
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

void ImportDialog::on_pushAddFile_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("Shamela book (*.bok);; All files (*.*)"));
    dialog.setViewMode(QFileDialog::Detail);

    if(dialog.exec())
        ui->fileListWidget->addItems(dialog.selectedFiles());
}

void ImportDialog::on_pushDeleteFile_clicked()
{
    foreach(QModelIndex index, ui->fileListWidget->selectionModel()->selectedIndexes())
        ui->fileListWidget->takeItem(index.row());
}

void ImportDialog::on_pushNext_clicked()
{
    if(ui->stackedWidget->currentIndex() == 0)
    {
        for(int i=0;i<ui->fileListWidget->count();i++){
            try {
                QList<ImportModelNode*> nodesList;
                getBookInfo(ui->fileListWidget->item(i)->text(), nodesList);

                foreach(ImportModelNode *node, nodesList)
                    m_model->appendNode(node, QModelIndex());

            } catch(QString &what) {
                QMessageBox::critical(this,
                                      trUtf8("خطأ عند الاستيراد"),
                                      what);
            }
        }

        ui->stackedWidget->setCurrentIndex(1);
    } else if(ui->stackedWidget->currentIndex() == 1){

        foreach(ImportModelNode *node, m_model->nodeFromIndex(QModelIndex())->childrenList()) {
             if(m_indexDB->addBook(node))
                qDebug() << "[+]" << node->getBookName();
            else
                qDebug() << "Error:" << node->getBookName();
        }
    }
}

void ImportDialog::getBookInfo(const QString &path, QList<ImportModelNode*> &nodes)
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

QString ImportDialog::getBookType(const QSqlDatabase &bookDB)
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
