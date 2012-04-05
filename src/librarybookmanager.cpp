#include "librarybookmanager.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "libraryinfo.h"
#include "modelenums.h"
#include "utils.h"
#include "xmlutils.h"
#include "authorsmanager.h"

#include <qdir.h>
#include <qstandarditemmodel.h>

static LibraryBookManager *m_instance=0;

LibraryBookManager::LibraryBookManager(QObject *parent) :
    DatabaseManager(parent),
    m_quranBook(0)
{
    QDir dataDir(MW->libraryInfo()->dataDir());
    setDatabasePath(dataDir.filePath("books.db"));

    m_authorsManager = AuthorsManager::instance();
    Q_CHECK_PTR(m_authorsManager);

    openDatabase();
    loadModels();

    m_instance = this;
}

LibraryBookManager::~LibraryBookManager()
{
    clear();
    qDeleteAll(m_usedBooks);

    m_instance = 0;
}

LibraryBookManager *LibraryBookManager::instance()
{
    return m_instance;
}

void LibraryBookManager::loadModels()
{
    loadLibraryBooks();
}

void LibraryBookManager::clear()
{
    // Delete only unused books
    QList<int> keys = m_books.keys();
    for(int i=0; i<keys.size(); i++) {
        int k = keys.at(i);
        LibraryBook *b = m_books.value(k);
        if(!m_usedBooks.contains(b))
            delete m_books.take(k);
    }

    m_books.clear();
}

void LibraryBookManager::loadLibraryBooks()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT id, title, type, authorID, author, info, bookFlags, indexFlags, filename "
                  "FROM books ORDER BY id");

    ML_QUERY_EXEC(query);

    while(query.next()) {
        LibraryBook *book = new LibraryBook();
        book->bookID = query.value(0).toInt();
        book->bookDisplayName = query.value(1).toString();
        book->bookType = static_cast<LibraryBook::Type>(query.value(2).toInt());
        book->bookInfo = query.value(5).toString();

        book->bookFlags = query.value(6).toInt();
        book->indexFlags = static_cast<LibraryBook::IndexFlags>(query.value(7).toInt());

        book->fileName = query.value(8).toString();
        book->bookPath = MW->libraryInfo()->bookPath(book->fileName);

        if(!book->isQuran()) {
            book->authorID = query.value(3).toInt();

            if(book->authorID) {
                book->authorName = m_authorsManager->getAuthorName(book->authorID);
            } else {
                book->authorName = query.value(4).toString();
            }
        }

        m_books.insert(book->bookID, book);
    }
}

QStandardItemModel *LibraryBookManager::getModel()
{
    QStandardItemModel *model = new QStandardItemModel();

    foreach(LibraryBook *book, m_books.values()) {
        QStandardItem *item = new QStandardItem();
        item->setText(book->bookDisplayName);
        item->setData(book->bookID, ItemRole::idRole);
        item->setIcon(QIcon(":/images/book.png"));

        model->appendRow(item);
    }

    model->setHorizontalHeaderLabels(QStringList() << tr("الكتب"));

    return model;
}

LibraryBook *LibraryBookManager::getLibraryBook(int bookID)
{
    LibraryBook *book = m_books.value(bookID);
    if(book)
        m_usedBooks.insert(book);
    else
        qWarning("No book with id %d", bookID);

    return book;
}

LibraryBook *LibraryBookManager::getQuranBook()
{
    if(!m_quranBook) {
        foreach (LibraryBook *book, m_books.values()) {
            if(book->isQuran()) {
                m_quranBook = book;
                m_usedBooks.insert(book);
                break;
            }
        }
    }

    return m_quranBook;
}

int LibraryBookManager::addBook(LibraryBook *book)
{
    QMutexLocker locker(&m_mutex);

    if(!book->bookID)
        book->bookID = getNewBookID();

    QSqlQuery query(m_db);

    Utils::QueryBuilder q;
    q.setTableName("books");
    q.setQueryType(Utils::QueryBuilder::Insert);

    q.addColumn("id", book->bookID);
    q.addColumn("title", book->bookDisplayName);
    q.addColumn("type", book->bookType);
    q.addColumn("authorID", book->authorID);
    q.addColumn("author", (!book->authorID) ? book->authorName : QVariant(QVariant::String));
    q.addColumn("info", book->bookInfo);
    q.addColumn("bookFlags", book->bookFlags);
    q.addColumn("indexFlags", book->indexFlags);
    q.addColumn("filename", book->fileName);

    ML_ASSERT_RET(q.exec(query), 0);

    m_books.insert(book->bookID, book);
    return book->bookID;
}

void LibraryBookManager::beginUpdate()
{
}

void LibraryBookManager::endUpdate()
{
    reloadModels();
}

bool LibraryBookManager::updateBook(LibraryBook *book)
{
    QSqlQuery query(m_db);

    Utils::QueryBuilder q;
    q.setTableName("books");
    q.setQueryType(Utils::QueryBuilder::Update);

    q.addColumn("title", book->bookDisplayName);
    q.addColumn("type", book->bookType);
    q.addColumn("authorID", book->authorID);
    q.addColumn("author", (!book->authorID) ? book->authorName : QVariant(QVariant::String));
    q.addColumn("info", book->bookInfo);
    q.addColumn("bookFlags", book->bookFlags);
    q.addColumn("indexFlags", book->indexFlags);
    q.addColumn("filename", book->fileName);

    q.addWhere("id", book->bookID);

    ML_ASSERT_RET(q.exec(query), false);

    m_books.insert(book->bookID, book); //FIXME: memory leak
    return true;
}

bool LibraryBookManager::removeBook(int bookID)
{
    m_query.prepare("DELETE FROM books WHERE id = ?");
    m_query.bindValue(0, bookID);
    if(m_query.exec()) {
        m_books.remove(bookID); //FIXME: memory leak
        return true;
    } else {
        LOG_SQL_ERROR(m_query);
        return false;
    }
}

QList<int> LibraryBookManager::getNonIndexedBooks()
{
    QList<int> list;
    QHash<int, LibraryBook*>::const_iterator i = m_books.constBegin();

    while (i != m_books.constEnd()) {
        LibraryBook *b=i.value();
        if(b->indexFlags == LibraryBook::NotIndexed) {
            list << i.key();
        }

        ++i;
    }

    return list;
}

void LibraryBookManager::setBookIndexStat(int bookID, LibraryBook::IndexFlags indexFlag)
{
    QSqlQuery query(m_db);

    Utils::QueryBuilder q;
    q.setTableName("books");
    q.setQueryType(Utils::QueryBuilder::Update);

    q.addColumn("indexFlags", indexFlag);
    q.addWhere("id", bookID);

    ML_ASSERT(q.exec(query));

    LibraryBook *book = m_books.value(bookID);
    ML_ASSERT2(book, "LibraryBookManager::setBookIndexStat No book with id" << bookID << "where found");

    book->indexFlags = indexFlag;
}

int LibraryBookManager::getNewBookID()
{
    int bookID =  0;

    do {
        bookID = Utils::randInt(111111, 999999);
    } while(m_books.contains(bookID));

    return bookID;
}

