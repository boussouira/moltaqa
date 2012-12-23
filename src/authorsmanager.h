#ifndef AUTHORSMANAGER_H
#define AUTHORSMANAGER_H

#include "databasemanager.h"
#include "authorinfo.h"
#include <qhash.h>

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

    int addAuthor(AuthorInfo::Ptr auth);
    void removeAuthor(int authorID);

    AuthorInfo::Ptr getAuthorInfo(int authorID);
    bool hasAuthorInfo(int authorID);
    QString getAuthorName(int authorID);

    AuthorInfo::Ptr findAuthor(QString name, bool fazzySearch);

    void updateAuthor(AuthorInfo::Ptr auth);

protected:
    int getNewAuthorID();
    void loadAuthorsInfo();

protected:
    QHash<int, AuthorInfo::Ptr> m_authors;
};

#endif // AUTHORSMANAGER_H
