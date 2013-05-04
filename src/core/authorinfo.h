#ifndef AUTHORINFO_H
#define AUTHORINFO_H

#include <qsharedpointer.h>

class QDomElement;
class XmlDomHelper;

class AuthorInfo
{
public:
    AuthorInfo();

    typedef QSharedPointer<AuthorInfo> Ptr;

    enum AuthorFlags {
        UnknowBirth = 0x2,
        UnknowDeath = 0x4,
        ALive     = 0x8
    };

    void fromDomElement(QDomElement &authorElement);
    void toDomElement(XmlDomHelper &domHeleper, QDomElement &authorElement);

    inline AuthorInfo *clone() { return new AuthorInfo(*this); }

public:
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
};

#endif // AUTHORINFO_H
