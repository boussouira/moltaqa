#include "shamelamapper.h"

ShamelaMapper::ShamelaMapper()
{
}

void ShamelaMapper::addCatMap(int shamelaID, int libID)
{
    m_catMap.insert(shamelaID, libID);
}

void ShamelaMapper::addAuthorMap(int shamelaID, int libID)
{
    m_authorsMap.insert(shamelaID, libID);
}

void ShamelaMapper::addBookMap(int shamelaID, int libID)
{
    QMutexLocker locker(&m_mutex);

    m_booksMap.insert(shamelaID, libID);
}

void ShamelaMapper::addPageMap(int shamelaBookID, int shamelaPageID, int libPageID)
{
    QMutexLocker locker(&m_mutex);

    m_bookPages[shamelaBookID].insert(shamelaPageID, libPageID);
}

int ShamelaMapper::mapFromShamelaCat(int catID)
{
    return m_catMap.value(catID, 0);
}

int ShamelaMapper::mapFromShamelaAuthor(int authID)
{
    return m_authorsMap.value(authID, 0);
}

int ShamelaMapper::mapFromShamelaBook(int bookID)
{
    return m_booksMap.value(bookID, 0);
}

int ShamelaMapper::mapFromShamelaPage(int shamelaBook, int shamelaPage)
{
    return m_bookPages[shamelaBook][shamelaPage];
}
