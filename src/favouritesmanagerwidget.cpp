#include "favouritesmanagerwidget.h"
#include "ui_favouritesmanagerwidget.h"
#include "favouritesmanager.h"
#include "modelutils.h"
#include "modelenums.h"
#include "utils.h"

#include <qabstractitemmodel.h>
#include <qmessagebox.h>
#include <qdebug.h>
#include <qmenu.h>
#include <qaction.h>
#include <qinputdialog.h>

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
