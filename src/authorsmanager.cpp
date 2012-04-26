#include "authorsmanager.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "utils.h"
#include "modelenums.h"
#include "xmlutils.h"
#include "timeutils.h"
#include "stringutils.h"
#include <qstandarditemmodel.h>
#include <qdir.h>

AuthorsManager::AuthorsManager(QObject *parent) :
    DatabaseManager(parent)
{
    QDir dataDir(MW->libraryInfo()->dataDir());
    setDatabasePath(dataDir.filePath("authors.db"));

    openDatabase();
}

AuthorsManager::~AuthorsManager()
{
    clear();
}

void AuthorsManager::loadModels()
{
    loadAuthorsInfo();
}

void AuthorsManager::clear()
{
    m_authors.clear();
}

void AuthorsManager::loadAuthorsInfo()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT id, name, full_name, info, birth_year, birth, "
                  "death_year, death, flags FROM authors ORDER BY id");

    ML_QUERY_EXEC(query);

    while(query.next()) {
        AuthorInfoPtr auth(new AuthorInfo());
        auth->id = query.value(0).toInt();
        auth->name = query.value(1).toString();
        auth->fullName = query.value(2).toString();
        auth->info = query.value(3).toString();

        int flags = query.value(8).toInt();
        if(flags & AuthorInfo::UnknowBirth) {
            auth->unknowBirth = true;
        } else {
            auth->birthYear = query.value(4).toInt();
            auth->birthStr = query.value(5).toString();
        }

        if(flags & AuthorInfo::ALive) {
            auth->isALive = true;
        } else if(flags & AuthorInfo::UnknowDeath) {
            auth->unknowDeath = true;
        } else {
            auth->deathYear = query.value(6).toInt();
            auth->deathStr = query.value(7).toString();
        }

        m_authors[auth->id] = auth;
    }

    ML_ASSERT2(m_authors.size(), "AuthorsManager: No author data where loaded");
}

QStandardItemModel *AuthorsManager::authorsModel()
{
    QStandardItemModel *model = new QStandardItemModel();

    model->setHorizontalHeaderLabels(QStringList() << tr("المؤلفين"));

    foreach(AuthorInfoPtr auth, m_authors.values()) {
        QStandardItem *item = new QStandardItem();
        item->setText(auth->name);
        item->setToolTip(auth->fullName);
        item->setData(auth->id, ItemRole::authorIdRole);
        model->appendRow(item);
    }

    return model;
}

int AuthorsManager::addAuthor(AuthorInfoPtr auth)
{
    QMutexLocker locker(&m_mutex);

    if(!auth->id)
        auth->id = getNewAuthorID();

    if(!auth->unknowBirth && auth->birthStr.isEmpty())
        auth->birthStr = Utils::Time::hijriYear(auth->birthYear);

    if(!auth->unknowDeath && !auth->isALive && auth->deathStr.isEmpty())
        auth->deathStr = Utils::Time::hijriYear(auth->deathYear);

    int flags = 0;
    if(auth->unknowBirth)
        flags |= AuthorInfo::UnknowBirth;
    if(auth->unknowDeath)
        flags |= AuthorInfo::UnknowDeath;
    if(auth->isALive)
        flags |= AuthorInfo::ALive;

    QueryBuilder q;
    q.setTableName("authors");
    q.setQueryType(QueryBuilder::Insert);

    q.set("id", auth->id);
    q.set("name", auth->name);
    q.set("full_name", auth->fullName);
    q.set("info", auth->info);

    q.set("birth_year", auth->birthYear);
    q.set("birth", auth->birthStr);
    q.set("death_year", auth->deathYear);
    q.set("death", auth->deathStr);
    q.set("flags", flags);

    ML_ASSERT_RET(q.exec(m_query), 0);

    m_authors.insert(auth->id, auth);
    return auth->id;
}

void AuthorsManager::removeAuthor(int authorID)
{
    m_query.prepare("DELETE FROM authors WHERE id = ?");
    m_query.bindValue(0, authorID);
    if(m_query.exec())
        m_authors.remove(authorID);
    else
        LOG_SQL_ERROR(m_query);
    }

int AuthorsManager::getNewAuthorID()
{
    int authorID = 0;
    do {
        authorID = Utils::Rand::number(11111, 99999);
    } while(m_authors.contains(authorID));

    return authorID;
}

AuthorInfoPtr AuthorsManager::getAuthorInfo(int authorID)
{
    return m_authors.value(authorID);
}

bool AuthorsManager::hasAuthorInfo(int authorID)
{
    return m_authors.value(authorID);
}

QString AuthorsManager::getAuthorName(int authorID)
{
    AuthorInfoPtr auth = m_authors.value(authorID);
    return auth ? auth->name : QString();
}

AuthorInfoPtr AuthorsManager::findAuthor(QString name, bool fazzy)
{
    AuthorInfoPtr auth;
    QHash<int, AuthorInfoPtr>::const_iterator i = m_authors.constBegin();

    while (i != m_authors.constEnd()) {
        QString authorName = i.value()->name;
        if(fazzy) {
            if(Utils::String::Arabic::contains(authorName, name)) {
                auth = i.value();
                break;
            } else if(Utils::String::Arabic::fuzzyContains(authorName, name)) {
                auth = i.value();
            }
        } else {
            if(Utils::String::Arabic::compare(authorName, name)) {
                auth = i.value();
                break;
            }
        }

        ++i;
    }

    return auth;
}

void AuthorsManager::updateAuthor(AuthorInfoPtr auth)
{
    QMutexLocker locker(&m_mutex);

    if(!auth->id)
        auth->id = getNewAuthorID();

    if(!auth->unknowBirth && auth->birthStr.isEmpty())
        auth->birthStr = Utils::Time::hijriYear(auth->birthYear);

    if(!auth->unknowDeath && !auth->isALive && auth->deathStr.isEmpty())
        auth->deathStr = Utils::Time::hijriYear(auth->deathYear);

    int flags = 0;
    if(auth->unknowBirth)
        flags |= AuthorInfo::UnknowBirth;
    if(auth->unknowDeath)
        flags |= AuthorInfo::UnknowDeath;
    if(auth->isALive)
        flags |= AuthorInfo::ALive;

    QSqlQuery query(m_db);

    QueryBuilder q;
    q.setTableName("authors");
    q.setQueryType(QueryBuilder::Update);

    q.set("name", auth->name);
    q.set("full_name", auth->fullName);
    q.set("info", auth->info);

    q.set("birth_year", auth->birthYear);
    q.set("birth", auth->birthStr);
    q.set("death_year", auth->deathYear);
    q.set("death", auth->deathStr);
    q.set("flags", flags);

    q.where("id", auth->id);

    ML_ASSERT(q.exec(query));

    m_authors.insert(auth->id, auth);
}
