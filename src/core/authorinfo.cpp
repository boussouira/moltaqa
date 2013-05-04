#include "authorinfo.h"
#include "utils.h"
#include "xmldomhelper.h"

#include <qdom.h>

AuthorInfo::AuthorInfo()  :
    id(0),
    deathYear(0),
    birthYear(0),
    isALive(false),
    unknowBirth(false),
    unknowDeath(false)
{
}

void AuthorInfo::fromDomElement(QDomElement &authorElement)
{
    id = authorElement.attribute("id").toInt();

    int flags = authorElement.attribute("flags").toInt();
    unknowBirth = (flags & AuthorInfo::UnknowBirth);
    unknowDeath = (flags & AuthorInfo::UnknowDeath);
    isALive = (flags & AuthorInfo::ALive);

    name = authorElement.firstChildElement("name").text();
    fullName = authorElement.firstChildElement("full-name").text();
    info = authorElement.firstChildElement("info").text();

    QDomElement birthElement = authorElement.firstChildElement("birth");
    birthStr = birthElement.text();
    birthYear = birthElement.attribute("year").toInt();

    QDomElement deathElement = authorElement.firstChildElement("death");
    deathStr = deathElement.text();
    deathYear = deathElement.attribute("year").toInt();
}

void AuthorInfo::toDomElement(XmlDomHelper &domHeleper, QDomElement &authorElement)
{
    int flags = 0;
    if(unknowBirth)
        flags |= AuthorInfo::UnknowBirth;
    if(unknowDeath)
        flags |= AuthorInfo::UnknowDeath;
    if(isALive)
        flags |= AuthorInfo::ALive;

    authorElement.setAttribute("id", id);
    authorElement.setAttribute("flags", flags);

    domHeleper.setElementText(authorElement, "name", name);
    domHeleper.setElementText(authorElement, "full-name", fullName);
    domHeleper.setElementText(authorElement, "info", info, true);

    domHeleper.setElementText(authorElement, "birth", birthStr).setAttribute("year", birthYear);
    domHeleper.setElementText(authorElement, "death", deathStr).setAttribute("year", deathYear);
}
