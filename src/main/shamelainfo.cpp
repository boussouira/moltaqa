#include "shamelainfo.h"
#include <qfile.h>

ShamelaInfo::ShamelaInfo()
{
}

ShamelaInfo::~ShamelaInfo()
{
}

void ShamelaInfo::setName(QString name)
{
    m_name = name;
}

void ShamelaInfo::setShamelaPath(QString path)
{
    m_shamelaPath = path;
}

QString ShamelaInfo::name()
{
    return m_name;
}

QString ShamelaInfo::shamelaPath()
{
    return m_shamelaPath;
}

QString ShamelaInfo::shamelaMainDbName()
{
    return QString("main.mdb");
}

QString ShamelaInfo::shamelaMainDbPath()
{
    return QString("%1/Files/%2").arg(shamelaPath()).arg(shamelaMainDbName());
}

QString ShamelaInfo::shamelaSpecialDbName()
{
    return QString("special.mdb");
}

QString ShamelaInfo::shamelaSpecialDbPath()
{
    return QString("%1/Files/%2").arg(shamelaPath()).arg(shamelaSpecialDbName());
}

bool ShamelaInfo::isShamelaPath(QString path)
{
    return QFile::exists(QString("%1/Files/%2").arg(path).arg(shamelaMainDbName()));
}

QString ShamelaInfo::indexDbName()
{
    return QString("book_index.db");
}

QString ShamelaInfo::buildFilePath(QString bkid, int archive)
{
    if(!archive)
        return QString("%1/Books/%2/%3.mdb").arg(m_shamelaPath).arg(bkid.right(1)).arg(bkid);
    else
        return QString("%1/Books/Archive/%2.mdb").arg(m_shamelaPath).arg(archive);
}

QString ShamelaInfo::buildFilePath(int bkid, int archive)
{
    return buildFilePath(QString::number(bkid), archive);
}
