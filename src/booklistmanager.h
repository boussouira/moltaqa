#ifndef BOOKLISTMANAGER_H
#define BOOKLISTMANAGER_H

#include "xmlmanager.h"
#include "librarybook.h"
#include <qhash.h>
#include <qicon.h>

class QStandardItemModel;
class QStandardItem;
class QXmlStreamWriter;
class QModelIndex;
class AuthorsManager;

class CategorieInfo {
public:
    CategorieInfo() : catID(0) {}
    CategorieInfo(int cid, const QString &ctitle) : catID(cid), title(ctitle) {}

    int catID;
    QString title;
};

class BookListManager : public XmlManager
{
    Q_OBJECT

public:
    BookListManager(QObject *parent=0);
    ~BookListManager();

    void loadModels();
    void clear();

    QStandardItemModel *bookListModel();
    QStandardItemModel *catListModel();

    int categoriesCount();
    int booksCount();

    int getNewCategorieID();

    CategorieInfo *findCategorie(const QString &cat);
    int addCategorie(const QString &title, int parentCat);
    void addBook(LibraryBookPtr book, int parentCat);

protected:
    void saveModel(QXmlStreamWriter &writer, QStandardItemModel *model);
    void readNode(QStandardItem *parentItem, QDomElement &element, bool withBooks=true);
    QList<QStandardItem*> readCatNode(QDomElement &element);
    QList<QStandardItem*> readBookNode(QDomElement &element);

    void writeItem(QXmlStreamWriter &writer, QModelIndex &index);

protected:
    AuthorsManager *m_authorsManager;
    QStandardItemModel *m_bookModel;
    QStandardItemModel *m_catModel;
    QHash<int, QString> m_catHash;
    QHash<int, QDomElement> m_catElementHash;
    int m_booksCount;
    int m_order;
    QIcon m_bookIcon;
    QIcon m_catIcon;
};

#endif // BOOKLISTMANAGER_H
