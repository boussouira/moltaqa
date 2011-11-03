#ifndef BOOKREADERHELPER_H
#define BOOKREADERHELPER_H

#include <qsqldatabase.h>
#include <qhash.h>
#include <bookindexmodel.h>
#include <qset.h>

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
    BookIndexModel *getBookModel(int bookID);
    void addBookModel(int bookID, BookIndexModel *model);
    void removeModel(int bookID);

protected:
    void open();
    void removeUnusedModel();

protected:
    QSqlDatabase m_quranDB;
    QHash<int, QuranSora*> m_sowar;
    QHash<int, BookIndexModel*> m_models;
    QSet<int> m_modelToDelete;
};

#endif // BOOKREADERHELPER_H
