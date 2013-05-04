#include "favouritesmanager.h"
#include "authorsmanager.h"
#include "librarybook.h"
#include "libraryinfo.h"
#include "librarymanager.h"
#include "modelenums.h"
#include "utils.h"
#include "xmlutils.h"

#include <qstandarditemmodel.h>
#include <qxmlstream.h>

FavouritesManager::FavouritesManager(QObject *parent) :
    BookListManager(parent)
{
    QDir dataDir(LibraryManager::instance()->libraryInfo()->dataDir());
    m_dom.setFilePath(dataDir.filePath("favourites.xml"));
}

FavouritesManager::~FavouritesManager()
{
}
