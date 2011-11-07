#include "sqlutils.h"
#include <qsqldatabase.h>
#include "mainwindow.h"
#include "bookreaderhelper.h"

namespace Utils {

DatabaseRemover::DatabaseRemover() :
    removeModel(-1)
{
}

DatabaseRemover::~DatabaseRemover()
{
    if(!connectionName.isEmpty()) {
        //qDebug("Remove database: %s", qPrintable(connectionName));
        QSqlDatabase::removeDatabase(connectionName);
    }

    if(removeModel != -1)
        MW->readerHelper()->removeModel(removeModel);
}

}
