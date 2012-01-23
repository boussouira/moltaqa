#ifndef BOOKREADERHELPER_H
#define BOOKREADERHELPER_H

#include <qsqldatabase.h>
#include <qhash.h>
#include <qset.h>
#include "sqlutils.h"

class QuranSora {
public:
    QuranSora() :
        sora(0),    ayatCount(0)
    {};

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

protected:
    void open();

protected:
    Utils::DatabaseRemover m_remover;
    QSqlDatabase m_quranDB;
    QHash<int, QuranSora*> m_sowar;
};

#endif // BOOKREADERHELPER_H
