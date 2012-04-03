#ifndef TARAJEMROWATMANAGER_H
#define TARAJEMROWATMANAGER_H

#include "xmlmanager.h"
#include "ziphelper.h"
#include <qhash.h>
#include <qset.h>

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

class TarajemRowatManager : public XmlManager
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

    bool beginUpdate();
    void endUpdate();

    void updateRawi(RawiInfo *rawi);
    int addRawi(RawiInfo *rawi);
    void removeRawi(int rawiID);

protected:
    int getNewRawiID();

protected:
    QString m_path;
    ZipHelper m_zip;
    QHash<int, RawiInfo*> m_info;
    QHash<int, RawiInfo*> m_fullInfo;
    QHash<int, QDomElement> m_elementHash;
    QList<QDomElement> m_newElements;
    QList<int> m_removedRowat;
    XmlDomHelperPtr m_domHelper;
};

#endif // TARAJEMROWATMANAGER_H
