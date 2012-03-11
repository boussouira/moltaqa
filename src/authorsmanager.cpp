#include "authorsmanager.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "utils.h"
#include "modelenums.h"
#include "xmlutils.h"
#include <qstandarditemmodel.h>
#include <qdir.h>

static AuthorsManager *m_instance=0;

AuthorsManager::AuthorsManager(QObject *parent) :
    ListManager(parent)
{
    QDir dataDir(MW->libraryInfo()->dataDir());
    m_dom.setFilePath(dataDir.filePath("authors.xml"));

    loadModels();

    m_instance = this;
}

AuthorsManager::~AuthorsManager()
{
    clear();

    m_instance = 0;
}

AuthorsManager *AuthorsManager::instance()
{
    return m_instance;
}

void AuthorsManager::loadModels()
{
    m_dom.load();

    loadAuthorsInfo();
}

void AuthorsManager::clear()
{
    qDeleteAll(m_authors);
    m_authors.clear();
}

QStandardItemModel *AuthorsManager::authorsModel()
{
    QStandardItemModel *model = new QStandardItemModel();

    model->setHorizontalHeaderLabels(QStringList() << tr("المؤلفين"));

    foreach(AuthorInfo *auth, m_authors.values()) {
        QStandardItem *item = new QStandardItem();
        item->setText(auth->name);
        item->setToolTip(auth->fullName);
        item->setData(auth->id, ItemRole::authorIdRole);

        model->appendRow(item);
    }

    return model;
}

int AuthorsManager::addAuthor(AuthorInfo *auth)
{
    QMutexLocker locker(&m_mutex);

    if(!auth->id)
        auth->id = getNewAuthorID();

    QDomElement authorElement = m_dom.domDocument().createElement("author");
    authorElement.setAttribute("id", auth->id);

    QDomElement nameEelement = m_dom.domDocument().createElement("name");
    QDomElement fullNameEelement = m_dom.domDocument().createElement("full-name");
    QDomElement infoEelement = m_dom.domDocument().createElement("info");

    nameEelement.appendChild(m_dom.domDocument().createTextNode(auth->name));
    fullNameEelement.appendChild(m_dom.domDocument().createTextNode(auth->fullName));
    infoEelement.appendChild(m_dom.domDocument().createCDATASection(auth->info));

    QDomElement birthElement = m_dom.domDocument().createElement("birth");
    if(auth->unknowBirth) {
        birthElement.setAttribute("unknow", "true");
    } else {
        birthElement.setAttribute("year", auth->birthYear);
        if(auth->birthStr.isEmpty())
            auth->birthStr = Utils::hijriYear(auth->birthYear);

        birthElement.appendChild(m_dom.domDocument().createTextNode(auth->birthStr));
    }

    QDomElement deathElement = m_dom.domDocument().createElement("death");
    deathElement.removeAttribute("aLive");
    deathElement.removeAttribute("unknow");

    if(auth->isALive) {
        deathElement.setAttribute("aLive", "true");
    } else if(auth->unknowDeath) {
        deathElement.setAttribute("unknow", "true");
    } else {
        deathElement.setAttribute("year", auth->deathYear);
        if(auth->deathStr.isEmpty())
            auth->deathStr = Utils::hijriYear(auth->deathYear);

        deathElement.appendChild(m_dom.domDocument().createTextNode(auth->deathStr));
    }

    authorElement.appendChild(nameEelement);
    authorElement.appendChild(fullNameEelement);
    authorElement.appendChild(birthElement);
    authorElement.appendChild(deathElement);
    authorElement.appendChild(infoEelement);

    m_dom.rootElement().appendChild(authorElement);
    m_authors.insert(auth->id, auth);

    m_dom.setNeedSave(true);

    return auth->id;
}

void AuthorsManager::removeAuthor(int authorID)
{
    QDomElement e = m_elementHash.value(authorID);

    if(!e.isNull()) {
        if(m_dom.rootElement().removeChild(e).isNull())
            qWarning("Error occured when removing author %d", authorID);

        m_dom.setNeedSave(true);
    }
}

int AuthorsManager::getNewAuthorID()
{
    int authorID = 0;
    do {
        authorID = Utils::randInt(11111, 99999);
    } while(m_authors.contains(authorID));

    return authorID;
}

AuthorInfo *AuthorsManager::getAuthorInfo(int authorID)
{
    return m_authors.value(authorID);
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

AuthorInfo *AuthorsManager::findAuthor(QString name)
{
    AuthorInfo *auth = 0;
    QHash<int, AuthorInfo*>::const_iterator i = m_authors.constBegin();

    while (i != m_authors.constEnd()) {
        QString authorName = i.value()->name;
        if(Utils::arContains(authorName, name)) {
            auth = i.value();
            break;
        } else if(Utils::arFuzzyContains(authorName, name)) {
            auth = i.value();
        }

        ++i;
    }

    return auth;
}

void AuthorsManager::beginUpdate()
{
    m_elementHash.clear();

    QDomElement e = m_dom.rootElement().firstChildElement();
    while(!e.isNull()) {
        m_elementHash.insert(e.attribute("id").toInt(), e);

        e = e.nextSiblingElement();
    }
}

void AuthorsManager::endUpdate()
{
    m_elementHash.clear();
    reloadModels();
}

void AuthorsManager::updateAuthor(AuthorInfo *auth)
{
    QDomElement e = m_elementHash.value(auth->id);

    if(!e.isNull()) {
        e.setAttribute("id", auth->id);

        m_dom.setElementText(e, "name", auth->name);
        m_dom.setElementText(e, "full-name", auth->fullName);
        m_dom.setElementText(e, "info", auth->info, true);

        QDomElement birthElement = Utils::findChildElement(e, m_dom.domDocument(), "birth");
        if(auth->unknowBirth)
            birthElement.setAttribute("unknow", "true");
        else
            birthElement.removeAttribute("unknow");

        birthElement.setAttribute("year", auth->birthYear);
        Utils::findChildText(birthElement, m_dom.domDocument()).setNodeValue(auth->birthStr);

        QDomElement deathElement = Utils::findChildElement(e, m_dom.domDocument(), "death");
        deathElement.removeAttribute("aLive");
        deathElement.removeAttribute("unknow");

        if(auth->isALive)
            deathElement.setAttribute("aLive", "true");
        else if(auth->unknowDeath)
            deathElement.setAttribute("unknow", "true");

        deathElement.setAttribute("year", auth->deathYear);
        Utils::findChildText(deathElement, m_dom.domDocument()).setNodeValue(auth->deathStr);

        m_dom.setNeedSave(true);
    }
}

void AuthorsManager::loadAuthorsInfo()
{
    QDomElement e = m_dom.rootElement().firstChildElement();
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
    auth->info = e.firstChildElement("info").text();

    QDomElement deathElement = e.firstChildElement("death");
    if(!deathElement.isNull()) {
        if(deathElement.hasAttribute("aLive") || deathElement.attribute("year").toInt()==99999) {
            auth->isALive = true;
        } else if(deathElement.hasAttribute("unknow") || !deathElement.hasAttribute("year")) {
            auth->unknowDeath = true;
        } else {
            auth->deathYear = deathElement.attribute("year").toInt();
            auth->deathStr = deathElement.hasChildNodes() ? deathElement.text()
                                                          : Utils::hijriYear(auth->deathYear);
            auth->unknowDeath = false;
        }
    } else {
        auth->unknowDeath = true;
    }

    QDomElement birthElement = e.firstChildElement("birth");
    if(!birthElement.isNull()
            && !birthElement.hasAttribute("unknow")
            && birthElement.hasAttribute("year")) {
        auth->birthYear = birthElement.attribute("year").toInt();
        auth->birthStr = birthElement.hasChildNodes() ? birthElement.text()
                                                      : Utils::hijriYear(auth->birthYear);
        auth->unknowBirth = false;
    } else {
        auth->unknowBirth = true;
    }

    m_authors.insert(auth->id, auth);
}
