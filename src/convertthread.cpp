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
#include "xmldomhelper.h"
#include "xmlutils.h"
#include "libraryinfo.h"
#include "bookutils.h"

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

    m_convertedFiles = m_files.size();

    foreach(QString file, m_files) {
        convert(file);
    }

    m_convertTime = time.elapsed();
}

void ConvertThread::convert(const QString &path)
{
    try {
        QFileInfo info(path);
        QString fileType = info.suffix().toLower();
        if(fileType == "bok")
            convertShamelaBook(path);
        else if(fileType == "mlp")
            convertMoltaqaPackage(path);
        else
            qWarning() << "ConvertThread: File" << info.fileName() << "not handeled";


    } catch(BookException &e) {
         e.print();
    }
}

void ConvertThread::convertShamelaBook(const QString &path)
{
#ifdef USE_MDBTOOLS
    MdbConverter mdb;
    QString tempFile = mdb.exportFromMdb(path);

    QString conn = "mdb_" + Utils::Rand::string(10, false);
    QSqlDatabase bookDB = QSqlDatabase::addDatabase("QSQLITE", conn);
    bookDB.setDatabaseName(tempFile);
#else
    QSqlDatabase bookDB = QSqlDatabase::addDatabase("QODBC", "mdb");
    bookDB.setDatabaseName(QString("DRIVER={Microsoft Access Driver (*.mdb)};FIL={MS Access};DBQ=%1").arg(path));
#endif

    if (!bookDB.open()) {
        ml_warn_db_error(bookDB);
        throw BookException(tr("لا يمكن فتح قاعدة البيانات"), path);
    }

    QSqlQuery bookQuery(bookDB);

    bookQuery.prepare("SELECT COUNT(*) FROM Main");
    ml_throw_on_query_exec_fail(bookQuery);

    if(bookQuery.next()) {
        int booksCount = bookQuery.value(0).toInt();
        if(booksCount > 1)
            emit addBooksToProgress(booksCount-1);
    }

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

         AuthorInfo::Ptr foundAuth = m_authorsManager->findAuthor(bookQuery.value(authCol).toString(), false);
         if(foundAuth)
             node->setAuthor(foundAuth->id, foundAuth->name);
         else
             node->setAuthor(0, bookQuery.value(authCol).toString());

        copyBookFromShamelaBook(node, bookDB, bookID);
        m_model->appendNode(node);
    }

    Utils::Sql::removeDatabase(bookDB);

#ifdef USE_MDBTOOLS
        if(tempFile.size()) {
            QFile::remove(tempFile);
            tempFile.clear();
        }
#endif
}

void ConvertThread::copyBookFromShamelaBook(ImportModelNode *node, const QSqlDatabase &bookDB, int bookID)
{
    NewBookWriter writer;
    writer.createNewBook();
    writer.startReading();

    QString tableName = QString("b%1").arg(bookID);
    QString queryFields = Utils::Book::shamelaQueryFields(bookDB, tableName);

    QSqlQuery query(bookDB);
    query.prepare(QString("SELECT %1 FROM %2 ORDER BY id")
                  .arg(queryFields)
                  .arg(tableName));

    ml_throw_on_query_exec_fail(query);

    int IdCol = query.record().indexOf("id");
    int nassCol = query.record().indexOf("nass");
    int pageCol = query.record().indexOf("page");
    int partCol = query.record().indexOf("part");
    int ayaCol = query.record().indexOf("aya");
    int soraCol = query.record().indexOf("sora");
    int hnoCol = query.record().indexOf("hno");

    BookPage page;
    while(query.next()) {
        page.pageID = query.value(IdCol).toInt();
        page.page = query.value(pageCol).toInt();
        page.part = query.value(partCol).toInt();

        if(soraCol != -1 && ayaCol != -1) {
            page.aya = query.value(ayaCol).toInt();
            page.sora = query.value(soraCol).toInt();
        }

        if(hnoCol != -1) {
            page.haddit = query.value(hnoCol).toInt();
        }

        writer.addPage(&page);
        page.clear();
    }


    query.prepare(QString("SELECT id, tit, lvl, sub FROM t%1 ORDER BY id, sub").arg(bookID));
    ml_throw_on_query_exec_fail(query);

    while(query.next()) {
        writer.addTitle(query.value(1).toString(),
                        query.value(0).toInt(),
                        query.value(2).toInt());
    }

    writer.writeMetaFiles();

    query.prepare(QString("SELECT id, nass FROM b%1").arg(bookID));
    ml_throw_on_query_exec_fail(query);

    IdCol = query.record().indexOf("id");
    nassCol = query.record().indexOf("nass");
    while(query.next()) {
        writer.addPageText(query.value(IdCol).toInt(),
                           query.value(nassCol).toString());
    }

    writer.endReading();

    node->path = writer.bookPath();

    emit bookConverted(node->title);
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

void ConvertThread::convertMoltaqaPackage(const QString &path)
{
    QFile zipFile(path);
    QuaZip zip(&zipFile);

    if(!zip.open(QuaZip::mdUnzip)) {
        qWarning("BookEditor::unZip cant Open zip file %d", zip.getZipError());
    }

    QuaZipFile contentFile(&zip);

    if(!zip.setCurrentFile("content.xml")) {
        qWarning("convertMoltaqaPackage: setCurrentFile error %d", zip.getZipError());
    }

    if(!contentFile.open(QIODevice::ReadOnly)) {
        qWarning("convertMoltaqaPackage: open error %d", contentFile.getZipError());
    }

    XmlDomHelper contentDom;
    contentDom.load(&contentFile);

    QDomElement books = contentDom.rootElement().firstChildElement("books");
    QDomElement authors = contentDom.rootElement().firstChildElement("authors");

    int booksCount = books.elementsByTagName("book").size();
    if(booksCount > 1)
        emit addBooksToProgress(booksCount-1);

    QDomElement bookElement = books.firstChildElement("book");
    while(!bookElement.isNull()) {
        extractMoltaqaBook(zip, bookElement, authors);

        bookElement = bookElement.nextSiblingElement("book");
    }
}

void ConvertThread::extractMoltaqaBook(QuaZip &zip, QDomElement &bookElement, QDomElement &authorsElement)
{
    ImportModelNode *node = new ImportModelNode(LibraryBook::NormalBook);
    node->fromDomElement(bookElement);
    node->setType(node->type);

    if(!node->isQuran()) {
        AuthorInfo::Ptr foundAuth = m_authorsManager->getAuthorInfo(node->authorID);
        if(foundAuth) {
            node->setAuthor(foundAuth->id, foundAuth->name);
        } else if(node->authorID) {
            // Import author's info from the package
            QDomElement authorElement = authorsElement.firstChildElement("author");
            while(!authorElement.isNull()) {
                if(authorElement.attribute("id").toInt() == node->authorID) {
                    AuthorInfo::Ptr author = importAuthorInfo(authorElement);
                    if(author) {
                        node->setAuthor(author->id, author->name);
                    }

                    break;
                }

                authorElement = authorElement.nextSiblingElement("author");
            }
        } else {
            // TODO: set the author to unknow
            qDebug() << "Author not found for book" << node->title;
        }
    }

    // Get categorie info
    node->setCategories(0);

    QDomElement categoriesElement = bookElement.firstChildElement("categories");
    if(!categoriesElement.isNull()
            && categoriesElement.childNodes().size()) {
        QDomElement cat = categoriesElement.firstChildElement("cat");
        // TODO: handle books with multi categories
        if (!cat.isNull()) {
            QScopedPointer<CategorieInfo> catInfo(m_bookListManager->findCategorie(cat.text(), false));
            if(catInfo) {
                node->setCategories(catInfo->catID, catInfo->title);
            } else {
                int newCatID = m_bookListManager->addCategorie(cat.text());
                node->setCategories(newCatID, cat.text());
            }
        }
    }

    QuaZipFile bookFile(&zip);

    if(!zip.setCurrentFile(node->fileName)) {
        qWarning("extractMoltaqaBook: setCurrentFile error %d", zip.getZipError());
    }

    if(!bookFile.open(QIODevice::ReadOnly)) {
        qWarning("extractMoltaqaBook: open error %d", bookFile.getZipError());
    }

    QDir tempDir(m_libraryManager->libraryInfo()->tempDir());
    node->path = (tempDir.exists(node->fileName)
                           ? Utils::Rand::fileName(tempDir.absolutePath(), true)
                           : tempDir.filePath(node->fileName));

    QFile outBookFile(node->path);
    if(!outBookFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "extractMoltaqaBook can't open" << outBookFile.fileName()
                   << "for writing:" << outBookFile.errorString();
    }

    Utils::Files::copyData(bookFile, outBookFile);

    m_model->appendNode(node);
}

AuthorInfo::Ptr ConvertThread::importAuthorInfo(QDomElement &authorElement)
{
    AuthorInfo::Ptr auth(new AuthorInfo());
    auth->fromDomElement(authorElement);

    int authorID = m_authorsManager->addAuthor(auth);
    return m_authorsManager->getAuthorInfo(authorID);
}
