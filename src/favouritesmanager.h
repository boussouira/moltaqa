#ifndef FAVOURITESMANAGER_H
#define FAVOURITESMANAGER_H

#include "booklistmanager.h"

class FavouritesManager : public BookListManager
{
    Q_OBJECT
public:
    FavouritesManager(QObject *parent = 0);
    ~FavouritesManager();

};

#endif // FAVOURITESMANAGER_H
