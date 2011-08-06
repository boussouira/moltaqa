#ifndef SHAMELAIMPORTINFO_H
#define SHAMELAIMPORTINFO_H

#include <qobject.h>
#include "shamelainfo.h"

class CategorieInfo
{
public:
    CategorieInfo(int cid, QString cname, int corder, int clevel) :
        id(cid),
        name(cname),
        order(corder),
        level(clevel) {}

    int id;
    QString name;
    int order;
    int level;
};

class AuthorInfo
{
public:
    AuthorInfo(int aid, int adieYear, QString aname, QString afullName, QString ainfo):
        id(aid),
        dieYear(adieYear),
        name(aname),
        fullName(afullName),
        info(ainfo) {}

    int id;
    int dieYear;
    QString name;
    QString fullName;
    QString info;
};

class ShamelaBookInfo
{
public:
    ShamelaBookInfo(int bid, QString bname, QString binfo, int barchive, int bcat, int bauth, QString bauthName);
    ~ShamelaBookInfo();

    void genInfo();
    void genInfo(ShamelaInfo *info);

public:
    int id;
    QString name;
    QString path;
    QString info;
    QString mainTable;
    QString tocTable;
    int archive;
    int cat;
    int authorID;
    QString authName;
};
#endif // SHAMELAIMPORTINFO_H
