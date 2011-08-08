#ifndef SHAMELAIMPORTTHREAD_H
#define SHAMELAIMPORTTHREAD_H

#include <qthread.h>
#include "librarycreator.h"

class ShamelaInfo;
class ShamelaManager;
class LibraryInfo;

class ShamelaImportThread : public QThread
{
    Q_OBJECT
public:
    ShamelaImportThread(QObject *parent = 0);
    void run();
    void stop();

protected:
    void importBooks();

signals:
    void bookImported(QString name);
    void doneImporting();

protected:
    ShamelaInfo *m_shamelaInfo;
    ShamelaManager *m_shamelaManager;
    LibraryCreator m_creator;
    bool m_stop;
    int m_threadID;
};

#endif // SHAMELAIMPORTTHREAD_H
