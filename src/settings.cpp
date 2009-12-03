#include "settings.h"

Settings::Settings(QWidget *parent): QWidget(parent)
{
    QString fontName;
    m_setting = new QSettings(CONFIGFILE, QSettings::IniFormat, this);

    fontName = m_setting->value("textBrowser/font", QVariant(DEFAUTFONT)).toString();
    m_dbPath = m_setting->value("app/db", QVariant(DATABASEPATH)).toString();
    m_textFont.fromString(fontName);

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
    m_setting->setValue("textBrowser/font", m_textFont.toString());
    m_setting->setValue("app/db", m_dbPath);
}

QString Settings::dbPath()
{
    return this->m_dbPath;
}

QFont Settings::textFont()
{
    return this->m_textFont;
}

void Settings::setDbPath(QString pNewDbPath)
{
    if(!pNewDbPath.isEmpty())
        m_dbPath = pNewDbPath;
}

void Settings::setTextFont(QFont pNewTextFont)
{
    m_textFont = pNewTextFont;
}
