#ifndef SHAMELAINFO_H
#define SHAMELAINFO_H

#include <QString>

class ShamelaInfo
{
public:
    ShamelaInfo();
    ~ShamelaInfo();
    void setName(QString name);
    void setShamelaPath(QString path);

    QString name();
    QString shamelaPath();

    QString shamelaMainDbName();
    QString shamelaMainDbPath();
    QString shamelaSpecialDbName();
    QString shamelaSpecialDbPath();
    bool isShamelaPath(QString path);

    QString indexDbName();
    QString indexDbPath();

    QString buildFilePath(QString bkid, int archive);
    QString buildFilePath(int bkid, int archive);

protected:
    QString m_name;
    QString m_path;
    QString m_shamelaPath;
    bool m_optimizeIndex;
    int m_ramSize;
};

#endif // SHAMELAINFO_H
