#ifndef AUTHORINFO_H
#define AUTHORINFO_H

#include <qsharedpointer.h>

class QDomElement;
class XmlDomHelper;

class AuthorInfo
{
public:
    AuthorInfo();

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

typedef QSharedPointer<AuthorInfo> AuthorInfoPtr;

#endif // AUTHORINFO_H
