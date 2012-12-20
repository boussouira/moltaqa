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
