#ifndef TARAJEMROWATMANAGER_H
#define TARAJEMROWATMANAGER_H

#include "databasemanager.h"
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

class TarajemRowatManager : public DatabaseManager
{
public:
    TarajemRowatManager(QObject *parent = 0);
    ~TarajemRowatManager();

    static TarajemRowatManager *instance();

    void loadModels();
    void clear();
    void reloadModels();

    QStandardItemModel *getRowatModel();

    RawiInfo *getRawiInfo(int rawiID);

    bool updateRawi(RawiInfo *rawi);
    int addRawi(RawiInfo *rawi);
    bool removeRawi(int rawiID);

protected:
    int getNewRawiID();

protected:
    QString m_path;
    QHash<int, RawiInfo*> m_rowat;
};

#endif // TARAJEMROWATMANAGER_H
