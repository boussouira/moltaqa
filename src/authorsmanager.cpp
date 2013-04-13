#include "authorsmanager.h"
#include "libraryinfo.h"
#include "mainwindow.h"
#include "modelenums.h"
#include "stringutils.h"
#include "timeutils.h"
#include "utils.h"
#include "xmlutils.h"

#include <qdir.h>
#include <qstandarditemmodel.h>

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

    ml_query_exec(query);

    while(query.next()) {
        AuthorInfo::Ptr auth(new AuthorInfo());
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
            auth->deathYear = Utils::Time::aliveDeathYear();
            auth->deathStr = tr("معاصر");
        } else if(flags & AuthorInfo::UnknowDeath) {
            auth->unknowDeath = true;
            auth->deathYear = Utils::Time::unknowDeathYear();
            auth->deathStr = tr("مجهول");
        } else {
            auth->deathYear = query.value(6).toInt();
            auth->deathStr = query.value(7).toString();
        }

        m_authors[auth->id] = auth;
    }

    ml_return_on_fail2(m_authors.size(), "AuthorsManager: No author data where loaded");
}

QStandardItemModel *AuthorsManager::authorsModel()
{
    QStandardItemModel *model = new QStandardItemModel();

    model->setHorizontalHeaderLabels(QStringList() << tr("المؤلفين"));

    foreach(AuthorInfo::Ptr auth, m_authors.values()) {
        QStandardItem *item = new QStandardItem();
        item->setText(auth->name);
        item->setToolTip(auth->fullName);
        item->setData(auth->id, ItemRole::authorIdRole);
        model->appendRow(item);
    }

    return model;
}

int AuthorsManager::authorsCount()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM authors");
    if(query.exec() && query.next())
        return query.value(0).toInt();

    return 0;
}

int AuthorsManager::addAuthor(AuthorInfo::Ptr auth)
{
    QMutexLocker locker(&m_mutex);

    if(!auth->id || getAuthorInfo(auth->id))
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

    ml_return_val_on_fail(q.exec(m_query), 0);

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
        ml_warn_query_error(m_query);
    }

int AuthorsManager::getNewAuthorID()
{
    int authorID = 0;
    do {
        authorID = Utils::Rand::number(11111, 99999);
    } while(getAuthorInfo(authorID));

    return authorID;
}

AuthorInfo::Ptr AuthorsManager::getAuthorInfo(int authorID)
{
    return m_authors.value(authorID);
}

bool AuthorsManager::hasAuthorInfo(int authorID)
{
    return m_authors.value(authorID);
}

QString AuthorsManager::getAuthorName(int authorID)
{
    AuthorInfo::Ptr auth = m_authors.value(authorID);
    return auth ? auth->name : QString();
}

QString AuthorsManager::formatAuthorName(QSharedPointer<AuthorInfo> author)
{
    if(!author->unknowBirth && !author->unknowDeath)
        return _u("%1 (%2 - %3)").arg(author->name).arg(author->birthStr.remove(_u(" هـ"))).arg(author->deathStr);
    else if(!author->unknowDeath)
        return _u("%1 (%2)").arg(author->name).arg(author->deathStr);
    else
        return author->name;
}

AuthorInfo::Ptr AuthorsManager::findAuthor(QString name, bool fazzySearch)
{
    if (name == QLatin1String("-"))
        return getAuthorInfo(unknowAuthorID());

    AuthorInfo::Ptr auth;
    QHash<int, AuthorInfo::Ptr>::const_iterator i = m_authors.constBegin();

    QString cleanName = Utils::String::Arabic::clean(name);

    while (i != m_authors.constEnd()) {
        QString authorName = i.value()->name;
        if(cleanName == Utils::String::Arabic::clean(authorName)) {
            auth = i.value();
            break;
        } else if(fazzySearch && Utils::String::Arabic::contains(authorName, name)) {
            auth = i.value();
        }

        ++i;
    }

    return auth;
}

void AuthorsManager::updateAuthor(AuthorInfo::Ptr auth)
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

    ml_return_on_fail(q.exec(query));

    m_authors.insert(auth->id, auth);
}
