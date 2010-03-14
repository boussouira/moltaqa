#include "tafessirtextbase.h"

TafessirTextBase::TafessirTextBase(QObject *parent) : QuranTextBase(parent)
{
    m_tafessirDBPath = "books/t-ibnkatir.db";

    m_tafessirDB = QSqlDatabase::addDatabase("QSQLITE", "QuranTafessirDB");
    m_tafessirDB.setDatabaseName(m_tafessirDBPath);

    if (!m_tafessirDB.open()) {
        qDebug() << "Cannot open database.";
    }
    m_tafessirQuery = new QSqlQuery(m_tafessirDB);
}
QString TafessirTextBase::getTafessirPage(PageInfo *pPageInfo)
{
    this->clearTafessirText();
    m_tafessirQuery->prepare("SELECT id, text, part, page, sora, aya "
                          "FROM tafesirText "
                          "WHERE sora = :m_sora AND aya = :m_aya "
                          "LIMIT 1");
    m_tafessirQuery->bindValue(":m_sora", pPageInfo->currentSoraNumber());
    m_tafessirQuery->bindValue(":m_aya", pPageInfo->currentAya());
    m_tafessirQuery->exec();
    while (m_tafessirQuery->next()) {
        this->appendTafessirText(m_tafessirQuery->value(1).toString());
    }
    return this->getTafessirText();
}
