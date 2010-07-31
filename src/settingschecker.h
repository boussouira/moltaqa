#ifndef SETTINGSCHECKER_H
#define SETTINGSCHECKER_H

#include <qobject.h>

class QSqlQuery;

class SettingsChecker : public QObject
{
public:
    SettingsChecker(QObject *parent = 0);
    void checkSettings();
    void createIndexBD(const QString &dbPath);
    void checkDefautQuran();

protected:
    void fillCatListTable(QSqlQuery *query);
};

#endif // SETTINGSCHECKER_H
