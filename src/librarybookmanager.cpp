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
    ListManager(parent),
    m_quranBook(0)
{
    QDir dataDir(MW->libraryInfo()->dataDir());
    m_dom.setFilePath(dataDir.filePath("booksinfo.xml"));
    m_authorsManager = AuthorsManager::instance();

    Q_CHECK_PTR(m_authorsManager);

    loadModels();
    beginUpdate();

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
    m_dom.load();

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
    m_bookElementHash.clear();
}

void LibraryBookManager::loadLibraryBooks()
{
    QDomElement e = m_dom.rootElement().firstChildElement();
    while(!e.isNull()) {
        readBook(e);

        e = e.nextSiblingElement();
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

void LibraryBookManager::addBook(LibraryBook *book)
{
    QMutexLocker locker(&m_mutex);

    if(!book->bookID)
        book->bookID = getNewBookID();

    QDomElement bookElement = m_dom.domDocument().createElement("book");
    bookElement.setAttribute("id", book->bookID);
    bookElement.setAttribute("type", book->bookType);
    bookElement.setAttribute("authorid", book->authorID);
    bookElement.setAttribute("bookFlags", book->bookFlags);
    bookElement.setAttribute("indexFlags", book->indexFlags);

    QDomElement titleElement = m_dom.domDocument().createElement("title");
    titleElement.appendChild(m_dom.domDocument().createTextNode(book->bookDisplayName));
    bookElement.appendChild(titleElement);

    QDomElement fileElement = m_dom.domDocument().createElement("filename");
    fileElement.appendChild(m_dom.domDocument().createTextNode(book->fileName));
    bookElement.appendChild(fileElement);

    QDomElement bookInfoElement = m_dom.domDocument().createElement("info");
    bookInfoElement.appendChild(m_dom.domDocument().createCDATASection(book->bookInfo));
    bookElement.appendChild(bookInfoElement);

    m_dom.rootElement().appendChild(bookElement);
    m_books.insert(book->bookID, book);

    m_dom.setNeedSave(true);
}

void LibraryBookManager::beginUpdate()
{
    m_bookElementHash.clear();

    QDomElement e = m_dom.rootElement().firstChildElement();
    while(!e.isNull()) {
        m_bookElementHash.insert(e.attribute("id").toInt(), e);

        e = e.nextSiblingElement();
    }
}

void LibraryBookManager::endUpdate()
{
    m_bookElementHash.clear();
    reloadModels();
}

void LibraryBookManager::updateBook(LibraryBook *book)
{
    QDomElement e = m_bookElementHash.value(book->bookID);

    if(!e.isNull()) {
        e.setAttribute("id", book->bookID);
        e.setAttribute("authorid", book->authorID);

        m_dom.setElementText(e, "title", book->bookDisplayName);
        m_dom.setElementText(e, "info", book->bookInfo, true);

        m_dom.setNeedSave(true);
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
    QDomElement e = m_bookElementHash.value(bookID);

    if(!e.isNull()) {
        e.setAttribute("indexFlags", static_cast<int>(indexFlag));

        LibraryBook *book = m_books.value(bookID);
        if(book)
            book->indexFlags = indexFlag;
        else qDebug("No book");

        m_dom.setNeedSave(true);
    } else qDebug("No element");
}

int LibraryBookManager::getNewBookID()
{
    int bookID =  0;

    do {
        bookID = Utils::randInt(111111, 999999);
    } while(m_books.contains(bookID));

    return bookID;
}

void LibraryBookManager::readBook(QDomElement &e)
{
    LibraryBook *book = new LibraryBook();
    book->bookID = e.attribute("id").toInt();
    book->bookType = static_cast<LibraryBook::Type>(e.attribute("type").toInt());
    book->bookDisplayName = e.firstChildElement("title").text();
    book->bookInfo = e.firstChildElement("info").text();
    book->fileName = e.firstChildElement("filename").text();
    book->bookPath = MW->libraryInfo()->bookPath(book->fileName);
    book->bookFlags = e.attribute("bookFlags").toInt();
    book->indexFlags = static_cast<LibraryBook::IndexFlags>(e.attribute("indexFlags").toInt());

    if(!book->isQuran()) {
        book->authorID = e.attribute("authorid").toInt();

        if(book->authorID) {
            book->authorName = m_authorsManager->getAuthorName(book->authorID);
        } else {
            QDomElement authorElement = e.firstChildElement("author");
            if(!authorElement.isNull())
                book->authorName = authorElement.text();
        }
    }

    m_books.insert(book->bookID, book);
}
