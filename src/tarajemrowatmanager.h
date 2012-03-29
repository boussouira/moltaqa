#ifndef TARAJEMROWATMANAGER_H
#define TARAJEMROWATMANAGER_H

#include "listmanager.h"
#include "ziphelper.h"
#include <qhash.h>

class RawiInfo
{
public:
    RawiInfo() : id(0) {}

    int id;
    QString name;
    QString tabaqa;
    QString rowat;
    QString rotba_hafed;
    QString rotba_zahabi;
    QString sheok;
    QString talamid;
    QString tarejama;
};

class TarajemRowatManager : public ListManager
{
public:
    TarajemRowatManager(QObject *parent = 0);
    ~TarajemRowatManager();

    static TarajemRowatManager *instance();

    void loadModels();
    void clear();

    QStandardItemModel *getRowatModel();

    RawiInfo *getRawiInfo(int rawiID);

protected:
    QString m_path;
    ZipHelper m_zip;
    QHash<int, RawiInfo*> m_info;
    QHash<int, RawiInfo*> m_fullInfo;
};

#endif // TARAJEMROWATMANAGER_H
