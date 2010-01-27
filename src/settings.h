#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QSettings>
#include <QFont>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>

#include "constant.h"

class Settings: public QWidget
{
public:
    Settings(QWidget *parent);
    ~Settings();
    QString dbPath();
    QFont textFont();
    void setDbPath(QString pNewDbPath);
    void setTextFont(QFont pNewTextFont);

private:
    QSettings *m_setting;
    QString m_dbPath;

};

#endif // SETTINGS_H
