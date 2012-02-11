#include "librarybookmanager.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "librarybook.h"
#include "modelenums.h"
#include "utils.h"
#include <qdir.h>
#include <qstandarditemmodel.h>

LibraryBookManager::LibraryBookManager(QObject *parent) :
    ListManager(parent),
    m_quranBook(0)
{
    QDir dataDir(MW->libraryInfo()->dataDir());
    m_filePath = dataDir.filePath("booksinfo.xml");

    loadLibraryBooks();
}

LibraryBookManager::~LibraryBookManager()
{
    clear();
}

void LibraryBookManager::loadLibraryBooks()
{
    clear();
    loadXmlDom();

    QDomElement e = m_rootElement.firstChildElement();
    while(!e.isNull()) {
        readBook(e);

        e = e.nextSiblingElement();
    }
}

void LibraryBookManager::clear()
{
    // TODO: what if an other object is using one of this books?
    qDeleteAll(m_books);
    m_books.clear();
}

void LibraryBookManager::reloadLibraryBooks()
{
    reloadXmlDom();
    loadLibraryBooks();
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
    if(!book)
        qWarning("No book with id %d", bookID);

    return book;
}

LibraryBook *LibraryBookManager::getQuranBook()
{
    if(!m_quranBook) {
        foreach (LibraryBook *book, m_books.values()) {
            if(book->isQuran()) {
                m_quranBook = book;
                break;
            }
        }
    }

    return m_quranBook;
}

void LibraryBookManager::addBook(LibraryBook *book)
{
    QMutexLocker locker(&m_mutex);

    QDomElement bookElement = m_doc.createElement("book");
    bookElement.setAttribute("id", book->bookID);
    bookElement.setAttribute("type", book->bookType);
    bookElement.setAttribute("authorid", book->authorID);
    bookElement.setAttribute("flags", book->flags);

    QDomElement titleElement = m_doc.createElement("title");
    titleElement.appendChild(m_doc.createTextNode(book->bookDisplayName));
    bookElement.appendChild(titleElement);

    QDomElement fileElement = m_doc.createElement("filename");
    fileElement.appendChild(m_doc.createTextNode(book->fileName));
    bookElement.appendChild(fileElement);

    QDomElement bookInfoElement = m_doc.createElement("info");
    bookInfoElement.appendChild(m_doc.createCDATASection(book->bookInfo));
    bookElement.appendChild(bookInfoElement);

    m_rootElement.appendChild(bookElement);
    m_saveDom = true;
}

void LibraryBookManager::beginUpdate()
{
    m_bookElementHash.clear();

    QDomElement e = m_rootElement.firstChildElement();
    while(!e.isNull()) {
        m_bookElementHash.insert(e.attribute("id").toInt(), e);

        e = e.nextSiblingElement();
    }
}

void LibraryBookManager::endUpdate()
{
    m_bookElementHash.clear();
    reloadLibraryBooks();
}

void LibraryBookManager::updateBook(LibraryBook *book)
{
    QDomElement e = m_bookElementHash.value(book->bookID);

    if(!e.isNull()) {
        e.setAttribute("id", book->bookID);
        e.setAttribute("authorid", book->authorID);

        e.firstChildElement("title").firstChild().setNodeValue(book->bookDisplayName);
        e.firstChildElement("info").firstChild().setNodeValue(book->bookInfo);

        m_saveDom = true;
    }
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
    book->bookPath = MW->libraryInfo()->bookPath(e.firstChildElement("filename").text());

    if(!book->isQuran()) {
        book->authorID = e.attribute("authorid").toInt();
        book->authorName = e.firstChildElement("author").text();
    }

    m_books.insert(book->bookID, book);
}
