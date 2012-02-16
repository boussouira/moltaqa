#include "booklistmanager.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "libraryinfo.h"
#include "librarybook.h"
#include "utils.h"
#include "xmlutils.h"
#include "modelenums.h"
#include "authorsmanager.h"

#include <qdir.h>
#include <qstandarditemmodel.h>
#include <qxmlstream.h>
#include <qtconcurrentrun.h>

BookListManager::BookListManager(QObject *parent)
    : ListManager(parent),
      m_bookModel(0),
      m_catModel(0),
      m_booksCount(0),
      m_order(0)
{
    QDir dataDir(MW->libraryInfo()->dataDir());
    m_filePath = dataDir.filePath("bookslist.xml");
    m_authorsManager = MW->libraryManager()->authorsManager();

    Q_CHECK_PTR(m_authorsManager);

    loadModels();
}

BookListManager::~BookListManager()
{
    clear();
}

void BookListManager::loadModels()
{
    loadXmlDom();

    bookListModel();
    catListModel();

    emit ModelsReady();
}

void BookListManager::clear()
{
    if(m_bookModel) {
        delete m_bookModel;
        m_bookModel = 0;
    }

    if(m_catModel) {
        delete m_catModel;
        m_catModel = 0;
    }

    m_catHash.clear();
    m_catElementHash.clear();
}

QStandardItemModel *BookListManager::bookListModel()
{
    if(!m_bookModel) {
        m_booksCount = 0;

        m_bookModel = new QStandardItemModel(this);
        m_bookModel->setHorizontalHeaderLabels(QStringList() << tr("الكتاب")
                                         << tr("المؤلف")
                                         << tr("وفاة المؤلف"));

        readNode(m_bookModel->invisibleRootItem(), m_rootElement);
    }

    return m_bookModel;
}

QStandardItemModel *BookListManager::catListModel()
{
    if(!m_catModel) {
        m_catHash.clear();
        m_catElementHash.clear();

        m_catModel = new QStandardItemModel();
        m_catModel->setHorizontalHeaderLabels(QStringList() << tr("القسم"));

        readNode(m_catModel->invisibleRootItem(), m_rootElement, false);
    }

    return m_catModel;
}

void BookListManager::saveModel(QXmlStreamWriter &writer, QStandardItemModel *model)
{
    writer.writeStartDocument();
    writer.writeStartElement("books-list");

    QModelIndex index = model->index(0, 0);

    while(index.isValid()) {
        writeItem(writer, index);

        index = index.sibling(index.row()+1, 0);
    }

    writer.writeEndElement();
    writer.writeEndDocument();
}

int BookListManager::categoriesCount()
{
    if(!m_catModel)
        catListModel();

    return m_catHash.size();
}

int BookListManager::booksCount()
{
    return m_booksCount;
}

int BookListManager::maxCategorieID()
{
    int catID = 0;

    foreach(int cid, m_catHash.keys()) {
        catID = qMax(catID, cid);
    }

    return catID;
}

QPair<int, QString> BookListManager::findCategorie(const QString &cat)
{
    QPair<int, QString> result;
    QHash<int, QString>::const_iterator i = m_catHash.constBegin();

    while (i != m_catHash.constEnd()) {
        QString title = i.value();
        if(Utils::arContains(title, cat)) {
            result = qMakePair(i.key(), i.value());
            break;
        } else if(Utils::arFuzzyContains(title, cat)) {
            result = qMakePair(i.key(), i.value());
        }

        ++i;
    }

    return result;
}

int BookListManager::addCategorie(const QString &title, int parentCat)
{
    QMutexLocker locker(&m_mutex);

    int catID = maxCategorieID() + 1;
    QDomElement catElement = m_doc.createElement("cat");
    catElement.setAttribute("id", catID);

    QDomElement titleElement = m_doc.createElement("title");
    titleElement.appendChild(m_doc.createTextNode(title));

    catElement.appendChild(titleElement);

    QDomElement parentElement = m_catElementHash.value(parentCat, m_rootElement);
    QDomElement newElement = parentElement.appendChild(catElement).toElement();

    m_catHash.insert(catID, title);
    m_catElementHash.insert(catID, newElement);

    m_saveDom = true;

    return catID;
}

void BookListManager::addBook(LibraryBook *book, int parentCat)
{
    // TODO: check if the author exist in the our authors list
    QMutexLocker locker(&m_mutex);

    QDomElement bookElement = m_doc.createElement("book");
    bookElement.setAttribute("id", book->bookID);
    bookElement.setAttribute("type", book->bookType);
    bookElement.setAttribute("authorid", book->authorID);

    QDomElement titleElement = m_doc.createElement("title");
    titleElement.appendChild(m_doc.createTextNode(book->bookDisplayName));
    bookElement.appendChild(titleElement);

    QDomElement bookInfoElement = m_doc.createElement("info");
    bookInfoElement.appendChild(m_doc.createCDATASection(book->bookInfo));
    bookElement.appendChild(bookInfoElement);

    QDomElement parentElement = m_catElementHash.value(parentCat, m_rootElement);
    parentElement.appendChild(bookElement);

    m_saveDom = true;
}

void BookListManager::readNode(QStandardItem *parentItem, QDomElement &element, bool withBooks)
{
    QList<QStandardItem*> rows;
    if(element.tagName() == "cat")
        rows = readCatNode(element);
    else if(withBooks && element.tagName() == "book")
        rows = readBookNode(element);

    if(!rows.isEmpty())
        parentItem->appendRow(rows);

    QStandardItem *newParentItem = rows.isEmpty() ? parentItem : rows.first();

    QDomElement child = element.firstChildElement();
    while(!child.isNull()) {
        if(child.tagName() == "book" || child.tagName() == "cat")
            readNode(newParentItem, child, withBooks);

        child = child.nextSiblingElement();
    }
}

QList<QStandardItem*> BookListManager::readCatNode(QDomElement &element)
{
    QString title = element.firstChildElement("title").text();
    int catID = element.attribute("id").toInt();

    QStandardItem *catItem = new QStandardItem();
    catItem->setText(title);
    catItem->setIcon(QIcon(":/images/book-cat.png"));
    catItem->setData(catID, ItemRole::idRole);
    catItem->setData(ItemType::CategorieItem, ItemRole::itemTypeRole);
    catItem->setData(++m_order, ItemRole::orderRole);

    m_catHash.insert(catID, title);
    m_catElementHash.insert(catID, element);

    return QList<QStandardItem*>() << catItem;
}

QList<QStandardItem*> BookListManager::readBookNode(QDomElement &element)
{
    QList<QStandardItem*> rows;
    QDomElement bookNameElement = element.firstChildElement("title");
    QDomElement infoElement = element.firstChildElement("info");

    int bookID = element.attribute("id").toInt();
    int authorID = element.attribute("authorid").toInt();

    LibraryBook::Type type = static_cast<LibraryBook::Type>(element.attribute("type").toInt());

    QStandardItem *nameItem = new QStandardItem();
    nameItem->setText(bookNameElement.text());
    nameItem->setToolTip(infoElement.text());
    nameItem->setIcon(QIcon(":/images/book.png"));
    nameItem->setData(bookID, ItemRole::idRole);
    nameItem->setData(ItemType::BookItem, ItemRole::itemTypeRole);
    nameItem->setData(type, ItemRole::typeRole);
    nameItem->setData(++m_order, ItemRole::orderRole);

    rows << nameItem;

    if(type != LibraryBook::QuranBook) {
        AuthorInfo *auth = m_authorsManager->getAuthorInfo(authorID);
        QString authName = auth ? auth->name : element.firstChildElement("author").text();
        int deathYear = auth ? auth->deathYear : element.firstChildElement("author").attribute("death").toInt();
        QString deathStr = auth ? auth->deathStr : Utils::hijriYear(deathYear);

        QStandardItem *authItem = new QStandardItem();
        authItem->setText(authName);
        authItem->setData(authorID, ItemRole::authorIdRole);

        QStandardItem *authDeathItem = new QStandardItem();
        authDeathItem->setText(deathStr);
        authDeathItem->setData(deathYear, ItemRole::authorDeathRole);

        rows << authItem << authDeathItem;
    }

    m_booksCount++;

    return rows;
}

void BookListManager::writeItem(QXmlStreamWriter &writer, QModelIndex &index)
{
    if(index.data(ItemRole::itemTypeRole) == ItemType::BookItem) {
        QModelIndex authorIndex = index.sibling(index.row(), 1);
        QModelIndex authorDeathIndex = index.sibling(index.row(), 2);

        int authorID = authorIndex.data(ItemRole::authorIdRole).toInt();
        bool isQuran = index.data(ItemRole::typeRole).toInt() == LibraryBook::QuranBook;

        writer.writeStartElement("book");
        writer.writeAttribute("id", index.data(ItemRole::idRole).toString());
        writer.writeAttribute("type", index.data(ItemRole::typeRole).toString());

        if(!isQuran) {
            writer.writeAttribute("authorid", authorIndex.data(ItemRole::authorIdRole).toString());

            if(!m_authorsManager->hasAuthorInfo(authorID)) {
                writer.writeStartElement("author");
                writer.writeAttribute("death", authorDeathIndex.data(ItemRole::authorDeathRole).toString());
                writer.writeCharacters(authorIndex.data(Qt::DisplayRole).toString());
                writer.writeEndElement();
            }
        }

        writer.writeTextElement("title", index.data(Qt::DisplayRole).toString());

    } else if(index.data(ItemRole::itemTypeRole) == ItemType::CategorieItem) {
        writer.writeStartElement("cat");
        writer.writeAttribute("id", index.data(ItemRole::idRole).toString());

        writer.writeTextElement("title", index.data(Qt::DisplayRole).toString());
    }

    QModelIndex child = index.child(0, 0);

    while(child.isValid()) {
        writeItem(writer, child);

        child = child.sibling(child.row()+1, 0);
    }

    writer.writeEndElement();
}
