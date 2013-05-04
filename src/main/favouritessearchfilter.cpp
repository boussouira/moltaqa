#include "favouritessearchfilter.h"
#include "booklistmanager.h"
#include "favouritesmanager.h"
#include "librarymanager.h"
#include "modelutils.h"
#include "utils.h"

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

