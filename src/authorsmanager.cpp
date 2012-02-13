#include "authorsmanager.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "utils.h"
#include <qdir.h>

AuthorsManager::AuthorsManager(QObject *parent) :
    ListManager(parent)
{
    QDir dataDir(MW->libraryInfo()->dataDir());
    m_filePath = dataDir.filePath("authors.xml");

    loadModels();
}

void AuthorsManager::loadModels()
{
    loadXmlDom();
    loadAuthorsInfo();
}

void AuthorsManager::clear()
{
    qDeleteAll(m_authors);
    m_authors.clear();
}

AuthorInfo *AuthorsManager::getAuthorInfo(int authorID)
{
    AuthorInfo *auth = m_authors.value(authorID);

    if(!auth)
        qWarning("No author with id %d", authorID);

    return auth;
}

bool AuthorsManager::hasAuthorInfo(int authorID)
{
    return m_authors.value(authorID);
}

QString AuthorsManager::getAuthorName(int authorID)
{
    AuthorInfo *auth = m_authors.value(authorID);
    return auth ? auth->name : QString();
}

void AuthorsManager::loadAuthorsInfo()
{
    QDomElement e = m_rootElement.firstChildElement();
    while(!e.isNull()) {
        readAuthor(e);

        e = e.nextSiblingElement();
    }
}

void AuthorsManager::readAuthor(QDomElement &e)
{
    AuthorInfo *auth = new AuthorInfo();
    auth->id = e.attribute("id").toInt();
    auth->name = e.firstChildElement("name").text();
    auth->fullName = e.firstChildElement("full-name").text();

    QDomElement deathElement = e.firstChildElement("death");
    auth->deathYear = deathElement.attribute("year").toInt();
    auth->deathStr = deathElement.hasChildNodes() ? deathElement.text()
                                                  : Utils::hijriYear(auth->deathYear);

    m_authors.insert(auth->id, auth);
}
