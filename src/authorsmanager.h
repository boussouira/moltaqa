#ifndef AUTHORSMANAGER_H
#define AUTHORSMANAGER_H

#include "databasemanager.h"
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

    enum AuthorFlags {
        UnknowBirth = 0x2,
        UnknowDeath = 0x4,
        ALive     = 0x8
    };

    AuthorInfo *clone()
    {
        return new AuthorInfo(*this);
    }
};

class AuthorsManager : public DatabaseManager
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

    void updateAuthor(AuthorInfo *auth);

protected:
    int getNewAuthorID();
    void loadAuthorsInfo();

protected:
    QHash<int, AuthorInfo*> m_authors;
};

#endif // AUTHORSMANAGER_H
