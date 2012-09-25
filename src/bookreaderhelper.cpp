#include "bookreaderhelper.h"
#include "utils.h"
#include "modelutils.h"

#include <qdir.h>
#include <qsqlquery.h>
#include <qvariant.h>
#include <qdebug.h>
#include <qstandarditemmodel.h>

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
        ml_warn_query_error(query);
    }

    return quranSora;
}

QStandardItemModel *BookReaderHelper::getBookModel(int bookID)
{
    return m_models.object(bookID);
}

bool BookReaderHelper::containsBookModel(int bookID)
{
    return m_models.contains(bookID);
}

void BookReaderHelper::addBookModel(int bookID, QStandardItemModel *model)
{
    m_models.insert(bookID, model);
}

void BookReaderHelper::removeBookModel(int bookID)
{
    if(m_models.contains(bookID))
        m_models.remove(bookID);
}

QString BookReaderHelper::getTitleText(int bookID, int titleID)
{
    ml_return_val_on_fail(m_models.contains(bookID), QString());

    QStandardItemModel *model = m_models.object(bookID);
    ml_return_val_on_fail(model, QString());

    QModelIndex index = Utils::Model::findModelIndex(model, titleID);
    ml_return_val_on_fail(index.isValid(), QString());

    return index.data().toString();
}

void BookReaderHelper::open()
{
    if(!m_quranDB.isOpen()) {
        QDir dataDir(App::dataDir());
        QString path = dataDir.filePath("quran-meta.db");

        m_quranDB = QSqlDatabase::addDatabase("QSQLITE", "helper_quran");
        m_quranDB.setDatabaseName(path);

        if(!m_quranDB.open()) {
            ml_warn_db_error(m_quranDB);
        }
    }
}
