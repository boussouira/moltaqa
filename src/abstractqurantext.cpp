#include "abstractqurantext.h"

AbstractQuranText::AbstractQuranText(QObject *parent) : QObject(parent)
{
}

void AbstractQuranText::openQuranDB(QString pQuranDBPath)
{
    m_quranDB = QSqlDatabase::addDatabase("QSQLITE", "QuranTextDB");
    m_quranDB.setDatabaseName(pQuranDBPath);

    if (!m_quranDB.open()) {
        qDebug() << "Cannot open database.";
    }
    m_quranQuery = new QSqlQuery(m_quranDB);
}

void AbstractQuranText::getSowarList(QStringListModel *pSowarModel)
{
    QStringList sowarNamesList;
    m_quranQuery->exec("SELECT * FROM QuranSowar");
    while (m_quranQuery->next())
    {
      sowarNamesList << QString("%1. %2")
              .arg(m_quranQuery->value(0).toString())
              .arg(m_quranQuery->value(1).toString() );
    }

    pSowarModel->setStringList(sowarNamesList);
}
