#ifndef TARAJEMROWATMANAGER_H
#define TARAJEMROWATMANAGER_H

#include "databasemanager.h"
#include "qsharedpointer.h"
#include <qhash.h>

class RawiInfo
{
public:
    RawiInfo() : id(0), birthYear(99999), deathYear(99999) {}

    int id;
    QString name;
    QString laqab;

    int birthYear;
    QString birthStr;

    int deathYear;
    QString deathStr;

    QString tabaqa;
    QString rowat;

    QString rotba_hafed;
    QString rotba_zahabi;

    QString sheok;
    QString talamid;
    QString tarejama;

    static const int UnknowYear = 99999;

    inline bool unknowBirth() { return (birthYear == UnknowYear); }
    inline bool unknowDeath() { return (deathYear == UnknowYear); }

    RawiInfo *clone() { return new RawiInfo(*this); }
};

typedef QSharedPointer<RawiInfo> RawiInfoPtr;

class TarajemRowatManager : public DatabaseManager
{
public:
    TarajemRowatManager(QObject *parent = 0);
    ~TarajemRowatManager();

    void loadModels();
    void clear();
    void reloadModels();

    QStandardItemModel *getRowatModel();
    int rowatCount();

    RawiInfoPtr getRawiInfo(int rawiID);

    bool updateRawi(RawiInfoPtr rawi);
    int addRawi(RawiInfoPtr rawi);
    bool removeRawi(int rawiID);

protected:
    int getNewRawiID();

protected:
    QString m_path;
    QHash<int, RawiInfoPtr> m_rowat;
};

#endif // TARAJEMROWATMANAGER_H
