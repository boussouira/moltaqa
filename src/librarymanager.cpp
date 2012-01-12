#include "librarymanager.h"
#include "libraryinfo.h"
#include "librarybook.h"
#include "bookexception.h"
#include "bookslistmodel.h"
#include "importmodel.h"
#include "utils.h"
#include "searchresult.h"
#include "editablecatslistmodel.h"
#include "editablebookslistmodel.h"

#include <qdebug.h>
#include <qsqlquery.h>
#include <qfile.h>
#include <qdir.h>
#include <qsqlerror.h>
#include <qdatetime.h>

LibraryManager::LibraryManager(LibraryInfo *info, QObject *parent) : QObject(parent)
{
    m_model = 0;
    m_libraryInfo = info;
    m_connName = "BooksIndexDB";

    connect(&m_watcher, SIGNAL(finished()), SLOT(booksListModelLoaded()));
}

LibraryManager::~LibraryManager()
{
    if(m_future.isRunning())
        m_future.waitForFinished();

    qDeleteAll(m_savedBook);
    m_savedBook.clear();
}

void LibraryManager::setConnectionInfo(LibraryInfo *info)
{
    m_libraryInfo = info;
}

LibraryInfo *LibraryManager::connectionInfo()
{
    return m_libraryInfo;
}

void LibraryManager::open()
{
    QString booksIndexPath = m_libraryInfo->booksIndexPath();

    if(!QFile::exists(booksIndexPath))
       qWarning("Can't find index database: %s", qPrintable(booksIndexPath));

    m_indexDB = QSqlDatabase::addDatabase("QSQLITE", m_connName);
    m_indexDB.setDatabaseName(booksIndexPath);

    if (!m_indexDB.open())
        throw BookException(tr("لم يمكن فتح قاعدة البيانات الأساسية"), m_indexDB.lastError().text());
}

void LibraryManager::loadBooksListModel()
{
    m_future = QtConcurrent::run(this, &LibraryManager::loadModel);
    m_watcher.setFuture(m_future);
}

void LibraryManager::transaction()
{
    m_indexDB.transaction();
}

bool LibraryManager::commit()
{
    return m_indexDB.commit();
}

void LibraryManager::loadModel()
{
    if(m_model)
        delete(m_model);

    m_modelTime.start();

    m_model = new BooksListModel();
    BooksListNode *firstNode = new BooksListNode(BooksListNode::Root);

    childCats(firstNode, 0, false);

    m_model->setRootNode(firstNode);
}

BooksListModel *LibraryManager::booksListModel()
{
    if(m_future.isRunning())
        m_future.waitForFinished();

    return m_model;
}

EditableBooksListModel *LibraryManager::editBooksListModel()
{
    EditableBooksListModel *model = new EditableBooksListModel();
    BooksListNode *firstNode = new BooksListNode(BooksListNode::Root);

    childCats(firstNode, 0, false);

    model->setRootNode(firstNode);

    return model;
}

void LibraryManager::booksListModelLoaded()
{
    qDebug("Load books model take %d ms", m_modelTime.elapsed());

    emit booksListModelLoaded(m_model);
}

BooksListModel *LibraryManager::catsListModel()
{
    BooksListModel *model = new BooksListModel();
    BooksListNode *firstNode = new BooksListNode(BooksListNode::Root);
    childCats(firstNode, 0, true);

    model->setRootNode(firstNode);

    return model;
}

EditableCatsListModel *LibraryManager::editCatsListModel()
{
    EditableCatsListModel *model = new EditableCatsListModel();
    BooksListNode *firstNode = new BooksListNode(BooksListNode::Root);
    childCats(firstNode, 0, true);

    model->setRootNode(firstNode);

    return model;
}

QStandardItemModel *LibraryManager::taffessirModel(bool allTafassir)
{
    QStandardItemModel *model = new QStandardItemModel();

    QSqlQuery query(m_indexDB);

    QString sql = "SELECT book_id, tafessir_name, show_tafessir, tafessir_order "
            "FROM tafassirList ";

    if(!allTafassir) {
        sql.append(QString("WHERE show_tafessir = %1 ").arg(Qt::Checked));
    }

    sql.append("ORDER BY tafessir_order, tafessir_name");

    if(query.exec(sql)) {
        while(query.next()) {
            QStandardItem *nameItem = new QStandardItem();
            nameItem->setText(query.value(1).toString());
            nameItem->setData(query.value(0).toInt(), ItemRole::idRole);

            QStandardItem *showItem = new QStandardItem();
            showItem->setCheckable(true);
            showItem->setCheckState(static_cast<Qt::CheckState>(query.value(2).toInt()));

            model->appendRow(QList<QStandardItem*>() << nameItem << showItem);
        }
    } else {
        LOG_SQL_ERROR(query);
    }

    model->setHorizontalHeaderLabels(QStringList()
                                     << tr("التفسير")
                                     << tr("عرض عند تصفح القرآن الكريم"));
    return model;
}

void LibraryManager::updateTafessir(int bookID, QString name, int order, int showInViewer)
{
    QSqlQuery query(m_indexDB);
    query.prepare("UPDATE tafassirList SET "
                  "tafessir_name = ?, "
                  "tafessir_order = ?, "
                  "show_tafessir = ? "
                  "WHERE book_id = ?");
    query.bindValue(0, name.trimmed());
    query.bindValue(1, order);
    query.bindValue(2, showInViewer);
    query.bindValue(3, bookID);

    if(!query.exec()) {
        LOG_SQL_ERROR(query);
    }
}

QPair<int, QString> LibraryManager::findCategorie(const QString &cat)
{
    QPair<int, QString> foundCat;
    int count = 0;
    int catID = 0;

    if(cat.isEmpty()) {
        foundCat.first = 0;
        return foundCat;
    }

    QSqlQuery bookQuery(m_indexDB);
    bookQuery.exec(QString("SELECT id, title FROM catList WHERE %1").arg(Sql::buildLikeQuery(cat, "title")));
    while(bookQuery.next()) {
        catID = bookQuery.value(0).toInt();
        foundCat.first = bookQuery.value(0).toInt();
        foundCat.second = bookQuery.value(1).toString();

        if(foundCat.second == cat) { // We got a match
            count = 1;
            break;
        } else {
            count++;
        }
    }

    if(count != 1) {
        foundCat.first = 0;
        foundCat.second.clear();
    }

    return foundCat;
}

QPair<int, QString> LibraryManager::findAuthor(const QString &name)
{
    QPair<int, QString> foundAuth;
    int count = 0;

    if(name.isEmpty()) {
        foundAuth.first = 0;
        return foundAuth;
    }

    QSqlQuery bookQuery(m_indexDB);
    if(bookQuery.exec(QString("SELECT id, name FROM authorsList WHERE %1").arg(Sql::buildLikeQuery(name, "name")))) {
        while(bookQuery.next()) {
            foundAuth.first = bookQuery.value(0).toInt();
            foundAuth.second = bookQuery.value(1).toString();

            if(name == bookQuery.value(1).toString()) {
                count = 1;
                break;
            } else {
                count++;
            }
        }
    } else {
        LOG_SQL_ERROR(bookQuery)
    }

    if(count != 1) {
        foundAuth.first = 0;
        foundAuth.second.clear();
    }

    return foundAuth;
}

LibraryBook *LibraryManager::getBookInfo(int bookID)
{
    LibraryBook *savedBook = m_savedBook.value(bookID, 0);

    if(savedBook)
        return savedBook;

    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("SELECT booksList.bookDisplayName, booksList.bookType, booksList.fileName, "
                      "booksList.bookFullName, booksList.bookOtherNames, booksList.bookInfo, "
                      "booksList.bookEdition, booksList.bookPublisher, booksList.bookMohaqeq, "
                      "booksList.authorID, authorsList.name "
                      "FROM booksList "
                      "LEFT JOIN authorsList ON authorsList.id = booksList.authorID "
                      "WHERE booksList.id = ? LIMIT 1");
    bookQuery.bindValue(0, bookID);

    if(bookQuery.exec()) {
        if(bookQuery.next()) {
            LibraryBook *bookInfo = new LibraryBook();

            bookInfo->bookDisplayName = bookQuery.value(0).toString();
            bookInfo->bookType = (LibraryBook::Type)bookQuery.value(1).toInt();
            bookInfo->bookID = bookID;
            bookInfo->bookPath = m_libraryInfo->bookPath(bookQuery.value(2).toString());

            if(bookInfo->isNormal() || bookInfo->isTafessir()) {
                bookInfo->textTable = "bookPages";
                bookInfo->indexTable = "bookIndex";
            } else if(bookInfo->isQuran()) {
                bookInfo->textTable = "quranText";

            } else {
                qWarning("getBookInfo: Unknow book type");
            }

            bookInfo->bookFullName = bookQuery.value(3).toString();
            bookInfo->bookOtherNames = bookQuery.value(4).toString();
            bookInfo->bookInfo = bookQuery.value(5).toString();
            bookInfo->bookEdition = bookQuery.value(6).toString();
            bookInfo->bookPublisher = bookQuery.value(7).toString();
            bookInfo->bookMohaqeq = bookQuery.value(8).toString();
            bookInfo->authorID = bookQuery.value(9).toInt();
            bookInfo->authorName = bookQuery.value(10).toString();

            bookInfo->hasShareeh = hasShareeh(bookID);

            m_savedBook.insert(bookID, bookInfo);

            return bookInfo;
        }
    } else {
        LOG_SQL_ERROR(bookQuery);
    }

    return 0;
}

bool LibraryManager::hasShareeh(int bookID)
{
    QSqlQuery bookQuery(m_indexDB);
    int count = 0;

    bookQuery.prepare("SELECT COUNT(*) FROM ShareehMeta WHERE "
                      "mateen_book = ?");
    bookQuery.bindValue(0, bookID);
    if(bookQuery.exec()) {
        if(bookQuery.next()) {
            count = bookQuery.value(0).toInt();
        }
    } else {
        LOG_SQL_ERROR(bookQuery);
    }

    return count;
}

LibraryBook *LibraryManager::getQuranBook()
{
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("SELECT id FROM booksList "
                      "WHERE bookType = ? "
                      "LIMIT 1");

    bookQuery.bindValue(0, LibraryBook::QuranBook);
    if(bookQuery.exec()) {
        if(bookQuery.next()) {
            return getBookInfo(bookQuery.value(0).toInt());
        }
    } else {
        LOG_SQL_ERROR(bookQuery);
    }

    return 0;
}

QList<QPair<int, QString> > LibraryManager::getTafassirList()
{
    QList<QPair<int, QString> > list;
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.exec("SELECT book_id, tafessir_name FROM tafassirList");
    while(bookQuery.next()) {
        list.append(qMakePair(bookQuery.value(0).toInt(), bookQuery.value(1).toString()));
    }

    return list;
}

QHash<int, QString> LibraryManager::getCategoriesList()
{
    QHash<int, QString> cats;
    QSqlQuery catQuery(m_indexDB);
    if(!catQuery.exec("SELECT id, title FROM catList "
                      "ORDER BY catOrder")) {
        LOG_SQL_ERROR(catQuery);
    }

    while(catQuery.next()) {
        cats.insert(catQuery.value(0).toInt(), catQuery.value(1).toString());
    }

    return cats;
}

int LibraryManager::addBook(ImportModelNode *book)
{
    QSqlQuery indexQuery(m_indexDB);
    QString newBookName = Utils::genBookName(m_libraryInfo->booksDir());
    QString newPath = m_libraryInfo->booksDir() + "/" + newBookName;

    indexQuery.prepare("INSERT INTO booksList (id, bookID, bookType, bookFlags, bookCat, "
                       "bookDisplayName, bookInfo, authorName, authorID, fileName, bookFolder, indexFlags) "
                       "VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    indexQuery.bindValue(0, 0);
    indexQuery.bindValue(1, book->type);
    indexQuery.bindValue(2, 0);
    indexQuery.bindValue(3, book->catID);
    indexQuery.bindValue(4, book->bookName);
    indexQuery.bindValue(5, book->bookInfo);
    indexQuery.bindValue(6, book->authorName);
    indexQuery.bindValue(7, book->authorID);
    indexQuery.bindValue(8, newBookName); // Add file name
    indexQuery.bindValue(9, QVariant(QVariant::String));
    indexQuery.bindValue(10, Enums::NotIndexed);

    if(QFile::copy(book->bookPath, newPath)){
        if(!QFile::remove(book->bookPath))
            qWarning() << "Can't remove:" << book->bookPath;
        if(indexQuery.exec()) {
            int lastID = indexQuery.lastInsertId().toInt();
            return lastID;
        } else {
            LOG_SQL_ERROR(indexQuery);
            QFile::remove(newPath);
            return -1;
        }
    } else {
        qWarning() << "Can't copy" << book->bookPath << "to" << newPath;
        return -1;
    }
}

void LibraryManager::childCats(BooksListNode *parentNode, int pID, bool onlyCats)
{
    if(!onlyCats)
        booksCat(parentNode, pID); // Start with books

    QSqlQuery catQuery(m_indexDB);
    if(!catQuery.exec(QString("SELECT id, title, catOrder, parentID FROM catList "
                           "WHERE parentID = %1 ORDER BY catOrder").arg(pID))) {
        LOG_SQL_ERROR(catQuery);
    }

    while(catQuery.next())
    {
        BooksListNode *catNode = new BooksListNode(BooksListNode::Categorie,
                                                   catQuery.value(1).toString(),
                                                   QString(),
                                                   catQuery.value(0).toInt());
        catNode->order = catQuery.value(2).toInt();

        childCats(catNode, catQuery.value(0).toInt(), onlyCats);

        parentNode->appendChild(catNode);
    }
}

void LibraryManager::booksCat(BooksListNode *parentNode, int catID)
{
    QSqlQuery bookQuery(m_indexDB);
    bookQuery.prepare("SELECT booksList.id, booksList.bookDisplayName, "
                      "booksList.bookType, booksList.bookInfo, "
                      "authorsList.name, authorsList.die_year "
                           "FROM booksList LEFT JOIN authorsList "
                           "ON authorsList.id = booksList.authorID "
                           "WHERE booksList.bookCat = ?");
    bookQuery.bindValue(0, catID);
    if(!bookQuery.exec()) {
        LOG_SQL_ERROR(bookQuery);
    }

    while(bookQuery.next())
    {
        BooksListNode *secondChild = new BooksListNode(BooksListNode::Book,
                                                       bookQuery.value(1).toString(),
                                                       bookQuery.value(4).toString(),
                                                       bookQuery.value(0).toInt());
        secondChild->bookType = (LibraryBook::Type)bookQuery.value(2).toInt();
        secondChild->infoToolTip = bookQuery.value(3).toString();
        secondChild->authorDeathYear = bookQuery.value(5).toInt();
        secondChild->authorDeath = Utils::hijriYear(secondChild->authorDeathYear);

        parentNode->appendChild(secondChild);
    }
}

void LibraryManager::updateBookMeta(LibraryBook *info, bool newBook)
{
    QSqlQuery bookQuery(m_indexDB);
    if(newBook) {
        bookQuery.prepare("INSERT INTO bookMeta (id, book_info, add_time, update_time) VALUES (?, ?, ?, ?)");
        bookQuery.bindValue(0, info->bookID);
        bookQuery.bindValue(1, info->toString());
        bookQuery.bindValue(2, QDateTime::currentDateTime().toTime_t());
        bookQuery.bindValue(3, QDateTime::currentDateTime().toTime_t());
    } else {
        bookQuery.prepare("INSERT INTO bookMeta (id, book_info, update_time) VALUES (?, ?, ?)");
        bookQuery.bindValue(0, info->bookID);
        bookQuery.bindValue(1, info->toString());
        bookQuery.bindValue(2, QDateTime::currentDateTime().toTime_t());
    }

    if(!bookQuery.exec()) {
        LOG_SQL_ERROR(bookQuery);
    }
}

void LibraryManager::getShoroohPages(LibraryBook *info, BookPage *page)
{
    qDeleteAll(info->shorooh);
    info->shorooh.clear();

    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("SELECT ShareehMeta.shareeh_book, ShareehMeta.shareeh_id, booksList.bookDisplayName "
                      "FROM ShareehMeta "
                      "LEFT JOIN booksList "
                      "ON booksList.id =  ShareehMeta.shareeh_book "
                      "WHERE ShareehMeta.mateen_book = ? AND ShareehMeta.mateen_id = ?");
    bookQuery.bindValue(0, info->bookID);
    bookQuery.bindValue(1, page->pageID);
    if(!bookQuery.exec()) {
        LOG_SQL_ERROR(bookQuery);
    }

    while(bookQuery.next()) {
        info->shorooh.append(new BookShorooh(bookQuery.value(0).toInt(),
                                             bookQuery.value(1).toInt(),
                                             bookQuery.value(2).toString()));
    }
}

void LibraryManager::updateCatTitle(int catID, QString title)
{
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("UPDATE catList SET title = ? WHERE id = ?");
    bookQuery.bindValue(0, title);
    bookQuery.bindValue(1, catID);
    if(!bookQuery.exec()) {
        LOG_SQL_ERROR(bookQuery);
    }
}

void LibraryManager::updateCatParent(int catID, int parentID)
{
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("UPDATE catList SET parentID = ? WHERE id = ?");
    bookQuery.bindValue(0, parentID);
    bookQuery.bindValue(1, catID);
    if(!bookQuery.exec()) {
        LOG_SQL_ERROR(bookQuery);
    }
}

void LibraryManager::updateCatOrder(int catID, int catOrder)
{
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("UPDATE catList SET catOrder = ? WHERE id = ?");
    bookQuery.bindValue(0, catOrder);
    bookQuery.bindValue(1, catID);
    if(!bookQuery.exec()) {
        LOG_SQL_ERROR(bookQuery);
    }
}

void LibraryManager::makeCatPlace(int parentID, int catOrder)
{
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("UPDATE catList SET catOrder = catOrder + 1 WHERE catOrder >= ? AND parentID = ?");
    bookQuery.bindValue(0, catOrder);
    bookQuery.bindValue(1, parentID);
    if(!bookQuery.exec()) {
        LOG_SQL_ERROR(bookQuery);
    }
}

int LibraryManager::booksCount(int catID)
{
    QSqlQuery bookQuery(m_indexDB);

    if(catID != -1) {
        bookQuery.prepare("SELECT COUNT(*) FROM booksList WHERE bookCat = ?");
        bookQuery.bindValue(0, catID);
    } else {
        bookQuery.prepare("SELECT COUNT(*) FROM booksList");
    }

    if(bookQuery.exec()) {
        if(bookQuery.next()) {
            return bookQuery.value(0).toInt();
        }
    } else {
        LOG_SQL_ERROR(bookQuery);
    }

    return 0;
}

int LibraryManager::addNewCat(const QString &title)
{
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("INSERT INTO catList (id, title) VALUES (NULL, ?)");
    bookQuery.bindValue(0, title);

    if(bookQuery.exec()) {
        return bookQuery.lastInsertId().toInt();
    } else {
        LOG_SQL_ERROR(bookQuery);
    }

    return 0;
}

void LibraryManager::removeCat(int catID)
{
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("DELETE FROM catList WHERE id = ?");
    bookQuery.bindValue(0, catID);
    if(!bookQuery.exec()) {
        LOG_SQL_ERROR(bookQuery);
    }
}

bool LibraryManager::moveCatBooks(int fromCat, int toCat)
{
    QSqlQuery bookQuery(m_indexDB);

    bookQuery.prepare("UPDATE booksList SET bookCat = ? WHERE bookCat = ?");
    bookQuery.bindValue(0, toCat);
    bookQuery.bindValue(1, fromCat);
    if(bookQuery.exec()) {
        return true;
    } else {
        LOG_SQL_ERROR(bookQuery);
        return false;
    }
}

QStandardItemModel *LibraryManager::getAuthorsListModel()
{
    QStandardItemModel *model = new QStandardItemModel();
    QStandardItem *item;
    QSqlQuery bookQuery(m_indexDB);
    bookQuery.prepare("SELECT id, name, full_name FROM authorsList");
    if(bookQuery.exec()) {
        while(bookQuery.next()) {
            item = new QStandardItem(bookQuery.value(1).toString());
            item->setData(bookQuery.value(0).toInt(), Qt::UserRole);
            item->setData(bookQuery.value(2).toString(), Qt::ToolTipRole);

            model->appendRow(item);
        }
    } else {
        LOG_SQL_ERROR(bookQuery);
    }

    return model;
}

void LibraryManager::updateBookInfo(LibraryBook *info)
{
    QSqlQuery bookQuery(m_indexDB);
    bookQuery.prepare("UPDATE booksList SET "
                          "bookDisplayName = ?, "
                          "bookFullName = ?, "
                          "bookOtherNames = ?, "
                          "bookInfo = ?, "
                          "bookEdition = ?, "
                          "bookPublisher = ?, "
                          "bookMohaqeq = ?, "
                          "authorID = ? "
                      "WHERE id = ?");
    bookQuery.bindValue(0, info->bookDisplayName);
    bookQuery.bindValue(1, info->bookFullName);
    bookQuery.bindValue(2, info->bookOtherNames);
    bookQuery.bindValue(3, info->bookInfo);
    bookQuery.bindValue(4, info->bookEdition);
    bookQuery.bindValue(5, info->bookPublisher);
    bookQuery.bindValue(6, info->bookMohaqeq);
    bookQuery.bindValue(7, info->authorID);
    bookQuery.bindValue(8, info->bookID);

    if(!bookQuery.exec()) {
        LOG_SQL_ERROR(bookQuery);
    }
}

void LibraryManager::setBookIndexStat(int bookID, Enums::indexFlags indexFlag)
{
    QSqlQuery bookQuery(m_indexDB);
    bookQuery.prepare("UPDATE booksList "
                      "SET indexFlags = ? "
                      "WHERE id = ?");

    bookQuery.bindValue(0, indexFlag);
    bookQuery.bindValue(1, bookID);

    if(!bookQuery.exec()) {
        LOG_SQL_ERROR(bookQuery);
    }
}

QList<int> LibraryManager::getNonIndexedBooks()
{
    QList<int> list;
    QSqlQuery bookQuery(m_indexDB);
    bookQuery.prepare("SELECT id FROM booksList "
                      "WHERE indexFlags = ?");

    bookQuery.bindValue(0, Enums::NotIndexed);

    if(bookQuery.exec()) {
        while(bookQuery.next()) {
            list << bookQuery.value(0).toInt();
        }
    } else {
        LOG_SQL_ERROR(bookQuery);
    }

    return list;
}

int LibraryManager::categoriesCount()
{
    QSqlQuery bookQuery(m_indexDB);

    if(bookQuery.exec("SELECT COUNT(*) FROM catList")) {
        if(bookQuery.next()) {
            return bookQuery.value(0).toInt();
        }
    } else {
        LOG_SQL_ERROR(bookQuery);
    }

    return 0;
}
