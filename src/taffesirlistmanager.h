#ifndef TAFFESIRLISTMANAGER_H
#define TAFFESIRLISTMANAGER_H

#include "listmanager.h"
#include <qstandarditemmodel.h>

class TaffesirListManager : public ListManager
{
    Q_OBJECT

public:
    TaffesirListManager(QObject *parent=0);

    QStandardItemModel* taffesirListModel(bool allTaffasir=false);
    void addTafessir(int bookID, QString taffesirName);

    void save(QStandardItemModel *taffesirModel);
};

#endif // TAFFESIRLISTMANAGER_H
