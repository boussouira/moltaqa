#include "settings.h"

Settings::Settings(QWidget *parent): QWidget(parent)
{
    m_setting = new QSettings(CONFIGFILE, QSettings::IniFormat, this);

    m_dbPath = m_setting->value("app/db", QVariant(DATABASEPATH)).toString();

    while (!QFile::exists(m_dbPath))
    {
        int rep = QMessageBox::critical(0,
                                        tr("Cannot open database"),
                                        tr("Unable to establish a database connection\n"
                                           "You need to select database."),
                                        QMessageBox::Yes | QMessageBox::No);
        if (rep == QMessageBox::No)
            exit(1);
        m_dbPath = QFileDialog::getOpenFileName(0,
                                                      tr("Select the database"), "",
                                                      tr("SQLITE (*.db);;All files (*.*)"));
        if(!m_dbPath.isEmpty())
            break;
    }

}

Settings::~Settings()
{
    m_setting->setValue("app/db", m_dbPath);
}

QString Settings::dbPath()
{
    return this->m_dbPath;
}


void Settings::setDbPath(QString pNewDbPath)
{
    if(!pNewDbPath.isEmpty())
        m_dbPath = pNewDbPath;
}
