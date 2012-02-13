#ifndef AUTHORSMANAGER_H
#define AUTHORSMANAGER_H

#include "listmanager.h"
#include <qhash.h>

class AuthorInfo
{
public:
    AuthorInfo() : id(0), deathYear(0) {}
    int id;
    QString name;
    QString fullName;
    int deathYear;
    QString deathStr;
    QString info;
};

class AuthorsManager : public ListManager
{
    Q_OBJECT

public:
    AuthorsManager(QObject *parent = 0);

    void loadModels();
    void clear();

    AuthorInfo *getAuthorInfo(int authorID);
    bool hasAuthorInfo(int authorID);
    QString getAuthorName(int authorID);

protected:
    void loadAuthorsInfo();
    void readAuthor(QDomElement &e);

protected:
    QHash<int, AuthorInfo*> m_authors;
};

#endif // AUTHORSMANAGER_H
