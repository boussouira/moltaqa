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
    if(m_quranDB.isOpen()) {
        QString conn = m_quranDB.databaseName();

        m_quranDB = QSqlDatabase();
        QSqlDatabase::removeDatabase(conn);
    }

    qDeleteAll(m_sowar);
    m_sowar.clear();

    qDeleteAll(m_models);
    m_models.clear();
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

BookIndexModel *BookReaderHelper::getBookModel(int bookID)
{
    return m_models.value(bookID, 0);
}

void BookReaderHelper::addBookModel(int bookID, BookIndexModel *model)
{
    m_models.insert(bookID, model);

    qDebug("Add model for book %d", bookID);

    if(m_modelToDelete.size() > 10)
        removeUnusedModel();
}

void BookReaderHelper::removeModel(int bookID)
{
    m_modelToDelete.insert(bookID);

    if(m_modelToDelete.size() > 10)
        removeUnusedModel();
}

void BookReaderHelper::removeUnusedModel()
{
    qDebug("Remove Unused Model");
    foreach(int bookID, m_modelToDelete) {
        bool delModel = true;
        QAbstractItemModel *model = m_models.value(bookID, 0);

        if(model) {
            QString bookConn(QString("book_i%1_").arg(bookID));
            qDebug() << QSqlDatabase::connectionNames();
            foreach(QString conn, QSqlDatabase::connectionNames()) {
                if(conn.startsWith(bookConn)) {
                    delModel = false;
                    break;
                }
            }

            if(delModel) {
                qDebug("Delete model for book %d", bookID);
                m_models.remove(bookID);
                m_modelToDelete.remove(bookID);

                delete model;
            } else {
                qDebug("Model for book %d is in use", bookID);
            }
        } else {
            qDebug("No model found for book %d", bookID);
            m_modelToDelete.remove(bookID);
        }
    }
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
