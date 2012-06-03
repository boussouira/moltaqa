#include "favouritessearchfilter.h"
#include "librarymanager.h"
#include "favouritesmanager.h"
#include "booklistmanager.h"
#include "utils.h"
#include "modelutils.h"

FavouritesSearchFilter::FavouritesSearchFilter(QObject *parent) :
    LibrarySearchFilter(parent)
{
    setForceFilter(true);
}

void FavouritesSearchFilter::loadModel()
{
    FavouritesManager *favouritesManager = LibraryManager::instance()->favouritesManager();
    QStandardItemModel *model = Utils::Model::cloneModel(favouritesManager->bookListModel());
    Utils::Model::setModelCheckable(model);

    setSourceModel(model);
}

