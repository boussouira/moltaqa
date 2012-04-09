#include "favouritesmanager.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "utils.h"
#include "xmlutils.h"
#include "modelenums.h"
#include "librarybook.h"
#include "authorsmanager.h"
#include <qstandarditemmodel.h>
#include <qxmlstream.h>

FavouritesManager::FavouritesManager(QObject *parent) :
    BookListManager(parent)
{
    QDir dataDir(MW->libraryInfo()->dataDir());
    m_dom.setFilePath(dataDir.filePath("favourites.xml"));
}

FavouritesManager::~FavouritesManager()
{
}
