#ifndef AUTHORSMANAGER_H
#define AUTHORSMANAGER_H

#include "databasemanager.h"
#include <qhash.h>
#include <qsharedpointer.h>

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

    static inline QString formatAuthorName(QString name, QString deathStr)
    {
        return QString::fromUtf8("%1، %2").arg(name).arg(deathStr);
    }

    static inline QString formatAuthorName(QSharedPointer<AuthorInfo> author)
    {
        return formatAuthorName(author->name, author->deathStr);
    }

    AuthorInfo *clone()
    {
        return new AuthorInfo(*this);
    }
};

typedef QSharedPointer<AuthorInfo> AuthorInfoPtr;

class AuthorsManager : public DatabaseManager
{
    Q_OBJECT

public:
    AuthorsManager(QObject *parent = 0);
    ~AuthorsManager();

    void loadModels();
    void clear();

    QStandardItemModel *authorsModel();
    int authorsCount();

    int addAuthor(AuthorInfoPtr auth);
    void removeAuthor(int authorID);

    AuthorInfoPtr getAuthorInfo(int authorID);
    bool hasAuthorInfo(int authorID);
    QString getAuthorName(int authorID);

    AuthorInfoPtr findAuthor(QString name, bool fazzySearch);

    void updateAuthor(AuthorInfoPtr auth);

protected:
    int getNewAuthorID();
    void loadAuthorsInfo();

protected:
    QHash<int, AuthorInfoPtr> m_authors;
};

#endif // AUTHORSMANAGER_H
