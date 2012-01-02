#include "sqlutils.h"
#include <qsqldatabase.h>
#include "mainwindow.h"
#include "bookreaderhelper.h"

namespace Utils {

DatabaseRemover::DatabaseRemover()
{
}

DatabaseRemover::~DatabaseRemover()
{
    if(!connectionName.isEmpty()) {
        //qDebug("Remove database: %s", qPrintable(connectionName));
        QSqlDatabase::removeDatabase(connectionName);
    }
}

}
