#include "booklistmanager.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "libraryinfo.h"
#include "librarybook.h"
#include "utils.h"
#include "xmlutils.h"
#include "timeutils.h"
#include "stringutils.h"
#include "modelenums.h"
#include "authorsmanager.h"
#include "librarybookmanager.h"

#include <qdir.h>
#include <qstandarditemmodel.h>
#include <qxmlstream.h>
#include <qtconcurrentrun.h>

BookListManager::BookListManager(QObject *parent)
    : XmlManager(parent),
      m_bookModel(0),
      m_catModel(0),
      m_order(0),
      m_lastCatId(0),
      m_bookIcon(QIcon(":/images/book.png")),
      m_catIcon(QIcon(":/images/book-cat.png"))
{
    QDir dataDir(MW->libraryInfo()->dataDir());
    m_dom.setFilePath(dataDir.filePath("bookslist.xml"));
    m_authorsManager = LibraryManager::instance()->authorsManager();

    Q_CHECK_PTR(m_authorsManager);
}

BookListManager::~BookListManager()
{
    clear();
}

void BookListManager::loadModels()
{
    m_dom.load();

    bookListModel();
    catListModel();

    emit ModelsReady();
}

void BookListManager::clear()
{
    ml_delete_check(m_bookModel);
    ml_delete_check(m_catModel);

    m_catHash.clear();
    m_catElementHash.clear();
    m_booksCatHash.clear();
}

QStandardItemModel *BookListManager::bookListModel()
{
    if(!m_bookModel) {
        m_booksCatHash.clear();

        m_bookModel = new QStandardItemModel(this);
        m_bookModel->setHorizontalHeaderLabels(QStringList() << tr("الكتاب") << tr("المؤلف"));

        readNode(m_bookModel->invisibleRootItem(), m_dom.rootElement());
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

        readNode(m_catModel->invisibleRootItem(), m_dom.rootElement(), false);
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
    return m_booksCatHash.size();
}

QList<CategorieInfo> BookListManager::bookCategorie(int bookID)
{
    QList<CategorieInfo> list;

    QString cats = m_booksCatHash[bookID];
    if(cats.size()) {
        foreach(QString cat, cats.split(';', QString::SkipEmptyParts)) {
            int catID = cat.toInt();
            if(m_catHash.contains(catID)) {
                list << CategorieInfo(catID, m_catHash[catID]);
            }
        }
    }

    return list;
}

int BookListManager::getNewCategorieID()
{
    int catID = 0;

    do {
        catID = Utils::Rand::number(11111, 99999);
    } while(m_catHash.contains(catID));

    return catID;
}

CategorieInfo *BookListManager::findCategorie(const QString &cat)
{
    CategorieInfo *result = 0;
    QHash<int, QString>::const_iterator i = m_catHash.constBegin();

    while (i != m_catHash.constEnd()) {
        QString title = i.value();
        if(Utils::String::Arabic::compare(title, cat)) {
            return new CategorieInfo(i.key(), i.value());
        } else if(Utils::String::Arabic::contains(title, cat)) {
            if(!result)
                result = new CategorieInfo();

            result->catID = i.key();
            result->title = i.value();
        }

        ++i;
    }

    return result;
}

int BookListManager::addCategorie(const QString &title, int parentCat)
{
    QMutexLocker locker(&m_mutex);

    int catID = getNewCategorieID();

    QDomElement catElement = m_dom.domDocument().createElement("cat");
    catElement.setAttribute("id", catID);

    QDomElement titleElement = m_dom.domDocument().createElement("title");
    titleElement.appendChild(m_dom.domDocument().createTextNode(title));

    catElement.appendChild(titleElement);

    QDomElement parentElement = m_catElementHash.value(parentCat, m_dom.rootElement());
    QDomElement newElement = parentElement.appendChild(catElement).toElement();

    m_catHash.insert(catID, title);
    m_catElementHash.insert(catID, newElement);

    m_dom.setNeedSave(true);

    return catID;
}

void BookListManager::addBook(LibraryBookPtr book, int parentCat)
{
    // TODO: check if the author exist in the our authors list
    QMutexLocker locker(&m_mutex);

    QDomElement bookElement = m_dom.domDocument().createElement("book");
    bookElement.setAttribute("id", book->id);
    bookElement.setAttribute("type", book->type);
    bookElement.setAttribute("authorid", book->authorID);

    QDomElement titleElement = m_dom.domDocument().createElement("title");
    titleElement.appendChild(m_dom.domDocument().createTextNode(book->title));
    bookElement.appendChild(titleElement);

    QDomElement bookInfoElement = m_dom.domDocument().createElement("info");
    bookInfoElement.appendChild(m_dom.domDocument().createCDATASection(book->comment));
    bookElement.appendChild(bookInfoElement);

    QDomElement parentElement = m_catElementHash.value(parentCat, m_dom.rootElement());
    if(book->isQuran())
        parentElement.insertBefore(bookElement, QDomNode());
    else
        parentElement.appendChild(bookElement);

    m_dom.setNeedSave(true);
}

void BookListManager::addBook(int bookID, int parentCat)
{
    LibraryBookPtr book = LibraryManager::instance()->bookManager()->getLibraryBook(bookID);
    if(book)
        addBook(book, parentCat);
}

bool BookListManager::removeBook(int bookID)
{
    QList<QDomElement> books = m_dom.treeFindElements("book", "id", bookID);

    ml_return_val_on_fail(books.size(), false);

    foreach (const QDomElement &e, books) {
        ml_return_val_on_fail(!e.isNull(), false);

        QDomNode parent = e.parentNode();
        ml_return_val_on_fail(!parent.isNull(), false);

        ml_return_val_on_fail(!parent.removeChild(e).isNull(), false);
    }

    m_dom.setNeedSave(true);
    return true;
}

bool BookListManager::containsBook(int bookID)
{
    return m_booksCatHash.contains(bookID);
}

void BookListManager::readNode(QStandardItem *parentItem, QDomElement &element, bool withBooks)
{
    QList<QStandardItem*> rows;
    if(element.tagName() == "cat")
        rows = readCatNode(element);
    else if(withBooks && element.tagName() == "book")
        rows = readBookNode(element);

    if(rows.size())
        parentItem->appendRow(rows);

    QStandardItem *newParentItem = rows.isEmpty() ? parentItem : rows.first();

    int currentCat = m_lastCatId;
    QDomElement child = element.firstChildElement();
    while(!child.isNull()) {
        if(child.tagName() == "book" || child.tagName() == "cat")
            readNode(newParentItem, child, withBooks);

        m_lastCatId = currentCat;

        child = child.nextSiblingElement();
    }
}

QList<QStandardItem*> BookListManager::readCatNode(QDomElement &element)
{
    QString title = element.firstChildElement("title").text();
    int catID = element.attribute("id").toInt();

    QStandardItem *catItem = new QStandardItem();
    catItem->setText(title);
    catItem->setIcon(m_catIcon);
    catItem->setData(catID, ItemRole::idRole);
    catItem->setData(ItemType::CategorieItem, ItemRole::itemTypeRole);
    catItem->setData(++m_order, ItemRole::orderRole);

    m_lastCatId = catID;
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
    nameItem->setIcon(m_bookIcon);
    nameItem->setData(bookID, ItemRole::idRole);
    nameItem->setData(ItemType::BookItem, ItemRole::itemTypeRole);
    nameItem->setData(type, ItemRole::typeRole);
    nameItem->setData(++m_order, ItemRole::orderRole);

    rows << nameItem;

    if(type != LibraryBook::QuranBook) {
        AuthorInfoPtr auth = m_authorsManager->getAuthorInfo(authorID);
        QString authName;
        int deathYear = 999999;
        QString deathStr;

        if(auth) {
            authName = auth->name;
            deathYear = auth->deathYear;
            deathStr = auth->deathStr;
        } else {
            authName = element.firstChildElement("author").text();
            deathYear = element.firstChildElement("author").attribute("death",
                                                                     QString::number(999999)).toInt();
            deathStr = Utils::Time::hijriYear(deathYear);
        }

        QStandardItem *authItem = new QStandardItem();
        authItem->setData(authorID, ItemRole::authorIdRole);
        authItem->setText(AuthorInfo::formatAuthorName(authName, deathStr));
        authItem->setData(deathYear, ItemRole::authorDeathRole);

        rows << authItem;
    }

    m_booksCatHash[bookID].append(QString("%1;").arg(m_lastCatId));

    return rows;
}

void BookListManager::writeItem(QXmlStreamWriter &writer, QModelIndex &index)
{
    if(index.data(ItemRole::itemTypeRole) == ItemType::BookItem) {
        QModelIndex authorIndex = index.sibling(index.row(), 1);

        int authorID = authorIndex.data(ItemRole::authorIdRole).toInt();
        bool isQuran = index.data(ItemRole::typeRole).toInt() == LibraryBook::QuranBook;

        writer.writeStartElement("book");
        writer.writeAttribute("id", index.data(ItemRole::idRole).toString());
        writer.writeAttribute("type", index.data(ItemRole::typeRole).toString());

        if(!isQuran) {
            writer.writeAttribute("authorid", authorIndex.data(ItemRole::authorIdRole).toString());

            if(!m_authorsManager->hasAuthorInfo(authorID)) {
                writer.writeStartElement("author");
                writer.writeAttribute("death", authorIndex.data(ItemRole::authorDeathRole).toString());
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
