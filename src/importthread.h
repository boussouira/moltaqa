#ifndef IMPORTTHREAD_H
#define IMPORTTHREAD_H

#include <qthread.h>
#include <qhash.h>

class ImportModel;
class BooksIndexDB;
class QGridLayout;
class QSignalMapper;

class ImportThread : public QThread
{
    Q_OBJECT

public:
    ImportThread(QObject *parent = 0);
    void setModel(ImportModel *model) { m_model = model;}
    void setIndexDB(BooksIndexDB *indexDB) { m_indexDB = indexDB;}
    void setSignalMapper(QSignalMapper *signalMapper) { m_signalMapper = signalMapper;}
    void setGridLayout(QGridLayout *gridLayout) { m_gridLayout = gridLayout;}
    int importTime() { return m_importTime; }
    int importedBooks() { return m_importedBooks; }
    QHash<int, QString> booksList() { return m_booksList; }
    void run();

signals:
    void setProgress(int prog);

protected:
    ImportModel *m_model;
    BooksIndexDB *m_indexDB;
    QGridLayout *m_gridLayout;
    QSignalMapper *m_signalMapper;
    QHash<int, QString> m_booksList;
    int m_importTime;
    int m_importedBooks;
};

#endif // IMPORTTHREAD_H
