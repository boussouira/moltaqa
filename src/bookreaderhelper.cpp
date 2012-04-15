#include "bookreaderhelper.h"
#include "utils.h"

#include <qdir.h>
#include <qsqlquery.h>
#include <qvariant.h>
#include <qdebug.h>

BookReaderHelper::BookReaderHelper()
{
    open();
}

BookReaderHelper::~BookReaderHelper()
{
    m_remover.removeDatabase(m_quranDB);

    qDeleteAll(m_sowar);
    m_sowar.clear();
}

QuranSora *BookReaderHelper::getQuranSora(int sora)
{
    QuranSora *quranSora = 0;
    quranSora = m_sowar.value(sora, 0);

    if(quranSora)
        return quranSora;

    QSqlQuery query(m_quranDB);
    query.prepare("SELECT SoraName, ayatCount "
                  "FROM quranSowar "
                  "WHERE id = ?");
    query.bindValue(0, sora);
    if(query.exec()) {
        if(query.next()) {
            quranSora = new QuranSora();
            quranSora->sora = sora;
            quranSora->name = query.value(0).toString();
            quranSora->ayatCount = query.value(1).toInt();

            m_sowar.insert(sora, quranSora);
        } else {
            qWarning("getQuranSora: No sora with the given id: %d", sora);
        }
    } else {
        LOG_SQL_ERROR(query);
    }

    return quranSora;
}

void BookReaderHelper::open()
{
    if(!m_quranDB.isOpen()) {
        QDir dataDir(App::dataDir());
        QString path = dataDir.filePath("quran-meta.db");

        m_quranDB = QSqlDatabase::addDatabase("QSQLITE", "helper_quran");
        m_quranDB.setDatabaseName(path);

        if(!m_quranDB.open()) {
            LOG_DB_ERROR(m_quranDB);
        }
    }
}
