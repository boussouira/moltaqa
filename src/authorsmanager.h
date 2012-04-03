#ifndef AUTHORSMANAGER_H
#define AUTHORSMANAGER_H

#include "xmlmanager.h"
#include <qhash.h>

class AuthorInfo
{
public:
    AuthorInfo() :
        id(0), deathYear(0), birthYear(0),
        isALive(false), unknowBirth(false), unknowDeath(false) {}
    int id;
    QString name;
    QString fullName;
    int deathYear;
    int birthYear;
    QString deathStr;
    QString birthStr;
    QString info;
    bool isALive;
    bool unknowBirth;
    bool unknowDeath;

    AuthorInfo *clone()
    {
        return new AuthorInfo(*this);
    }
};

class AuthorsManager : public XmlManager
{
    Q_OBJECT

public:
    AuthorsManager(QObject *parent = 0);
    ~AuthorsManager();

    static AuthorsManager *instance();

    void loadModels();
    void clear();

    QStandardItemModel *authorsModel();

    int addAuthor(AuthorInfo *auth);
    void removeAuthor(int authorID);

    AuthorInfo *getAuthorInfo(int authorID);
    bool hasAuthorInfo(int authorID);
    QString getAuthorName(int authorID);

    AuthorInfo *findAuthor(QString name);

    void beginUpdate();
    void endUpdate();
    void updateAuthor(AuthorInfo *auth);

protected:
    int getNewAuthorID();
    void loadAuthorsInfo();
    void readAuthor(QDomElement &e);

protected:
    QHash<int, AuthorInfo*> m_authors;
    QHash<int, QDomElement> m_elementHash;
};

#endif // AUTHORSMANAGER_H
