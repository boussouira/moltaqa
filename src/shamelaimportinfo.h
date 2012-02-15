#ifndef SHAMELAIMPORTINFO_H
#define SHAMELAIMPORTINFO_H

#include <qobject.h>
#include "shamelainfo.h"
#include "authorsmanager.h"

class ShamelaCategorieInfo
{
public:
    ShamelaCategorieInfo(int cid, QString cname, int corder, int clevel) :
        id(cid),
        name(cname),
        order(corder),
        level(clevel) {}

    int id;
    QString name;
    int order;
    int level;
};

class ShamelaAuthorInfo
{
public:
    ShamelaAuthorInfo(int aid, int adieYear, QString aname, QString afullName, QString ainfo):
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

    AuthorInfo *toAuthorInfo()
    {
        AuthorInfo *author = new AuthorInfo;
        author->name = name;
        author->fullName = fullName;
        author->info = info;
        author->deathYear = dieYear;
        author->unknowBirth = true;

        return author;
    }
};

class ShamelaBookInfo
{
public:
    ShamelaBookInfo(int bid, QString bname, QString binfo, int barchive, int bcat, int bauth, QString bauthName, QString btafessirName);
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
    QString tafessirName;
};

class ShamelaShareehInfo
{
public:
    ShamelaShareehInfo(int mateenID, int mateenPage, int shareehID, int shareehPage)
        : mateen_id(mateenID), mateen_page(mateenPage), shareeh_id(shareehID), shareeh_page(shareehPage) {}
    int mateen_id;
    int mateen_page;
    int shareeh_id;
    int shareeh_page;
};

#endif // SHAMELAIMPORTINFO_H
