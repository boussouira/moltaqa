#ifndef SHAMELAMAPPER_H
#define SHAMELAMAPPER_H

#include <qhash.h>
#include <qmutex.h>

class ShamelaMapper
{
public:
    ShamelaMapper();

    void addCatMap(int shamelaID, int libID);
    void addAuthorMap(int shamelaID, int libID);
    void addBookMap(int shamelaID, int libID);

    /// Convert the given shamela categorie id to this library categorie
    int mapFromShamelaCat(int catID);
    /// Convert the given shamela author id to this library author id
    int mapFromShamelaAuthor(int authID);
    /// Convert the given shamela book id to this library book id
    int mapFromShamelaBook(int bookID);

protected:
    QHash<int, int> m_booksMap; ///< Map between shamela and our book's ids
    QHash<int, int> m_catMap;  ///< Map between shamela and our categorie's ids
    QHash<int, int> m_authorsMap;
    QHash<int, QHash<int, int> > m_bookPages;
    QMutex m_mutex;
};

#endif // SHAMELAMAPPER_H
