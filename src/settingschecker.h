#ifndef SETTINGSCHECKER_H
#define SETTINGSCHECKER_H

#include <qobject.h>
#include <qsettings.h>

class QSqlQuery;

class SettingsChecker : public QObject
{
public:
    SettingsChecker(QObject *parent = 0);
    void checkSettings();
    void createIndexBD(const QString &dbPath);
    void checkDefautQuran();
    void checkDefautStyle();

protected:
    void fillCatListTable(QSqlQuery *query);
    QSettings m_settings;

};

#endif // SETTINGSCHECKER_H
