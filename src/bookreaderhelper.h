#ifndef BOOKREADERHELPER_H
#define BOOKREADERHELPER_H

#include <qsqldatabase.h>
#include <qhash.h>
#include <qset.h>
#include <qcache.h>

#include "sqlutils.h"

class QStandardItemModel;

class QuranSora {
public:
    QuranSora() : sora(0), ayatCount(0) {}

    int sora;
    int ayatCount;
    QString name;
};

class BookReaderHelper
{
public:
    BookReaderHelper();
    ~BookReaderHelper();

    QuranSora *getQuranSora(int sora);
    QStandardItemModel *getBookModel(int bookID);
    bool containsBookModel(int bookID);
    void addBookModel(int bookID, QStandardItemModel *model);
    void removeBookModel(int bookID);

    QString getTitleText(int bookID, int titleID, bool parent);
    static QString formatTitlesList(QStringList &list);

protected:
    void open();

protected:
    DatabaseRemover m_remover;
    QSqlDatabase m_quranDB;
    QHash<int, QuranSora*> m_sowar;
    QCache<int, QStandardItemModel> m_models;
};

#endif // BOOKREADERHELPER_H
