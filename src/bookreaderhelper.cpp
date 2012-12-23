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

QString BookReaderHelper::getTitleText(int bookID, int titleID, bool parent)
{
    ml_return_val_on_fail(m_models.contains(bookID), QString());

    QStandardItemModel *model = m_models.object(bookID);
    ml_return_val_on_fail(model, QString());

    QModelIndex index = Utils::Model::findModelIndex(model, titleID);
    ml_return_val_on_fail(index.isValid(), QString());

    QStringList list;
    do {
        list << index.data().toString();

        index = index.parent();
    } while (index.isValid() && parent);

    return BookReaderHelper::formatTitlesList(list);
}

QString BookReaderHelper::formatTitlesList(QStringList &list)
{
    QString title;

    for(int i=list.size()-1; i>=0; i--) {
        title.append(list[i]);

        if(i)
            title.append("<span style=\"font-family:cursive;font-size:0.8em;color:#777;\"> > </span>");
    }

    return title.trimmed();
}

QString BookReaderHelper::formatTitlesList(QStack<QString> &stack)
{
    QList<QString> list(stack.toList());
    QString title;

    for(int i=0; i<list.size(); i++) {
        if(i)
            title.append("<span style=\"font-family:cursive;font-size:0.8em;color:#777;\"> > </span>");

        title.append(list[i]);
    }

    return title.trimmed();
}

void BookReaderHelper::open()
{
    if(!m_quranDB.isOpen()) {
        QDir dir(App::dataDir());
        QString path = dir.filePath("quran-meta.db");

        m_quranDB = QSqlDatabase::addDatabase("QSQLITE", "helper_quran");
        m_quranDB.setDatabaseName(path);

        if(!m_quranDB.open()) {
            ml_warn_db_error(m_quranDB);
        }
    }
}
