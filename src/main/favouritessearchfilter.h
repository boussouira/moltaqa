#ifndef FAVOURITESSEARCHFILTER_H
#define FAVOURITESSEARCHFILTER_H

#include "librarysearchfilter.h"

class FavouritesSearchFilter : public LibrarySearchFilter
{
    Q_OBJECT

public:
    FavouritesSearchFilter(QObject *parent=0);

    void loadModel();
};

#endif // FAVOURITESSEARCHFILTER_H
