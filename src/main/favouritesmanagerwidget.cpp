#include "favouritesmanagerwidget.h"
#include "ui_favouritesmanagerwidget.h"
#include "favouritesmanager.h"
#include "librarymanager.h"
#include "modelenums.h"
#include "modelutils.h"
#include "utils.h"

#include <qabstractitemmodel.h>
#include <qaction.h>
#include <qinputdialog.h>
#include <qmenu.h>
#include <qmessagebox.h>

FavouritesManagerWidget::FavouritesManagerWidget(QWidget *parent) :
    BookListManagerWidget(parent)
{
     m_manager = LibraryManager::instance()->favouritesManager();
}

FavouritesManagerWidget::~FavouritesManagerWidget()
{
}

QString FavouritesManagerWidget::title()
{
    return tr("المفضلة");
}
