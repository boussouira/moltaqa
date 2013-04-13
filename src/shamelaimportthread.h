#ifndef SHAMELAIMPORTTHREAD_H
#define SHAMELAIMPORTTHREAD_H

#include "librarycreator.h"

#include <qthread.h>

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
    void setImportQuran(bool importQuran) { m_importQuran = importQuran; }
    void setThreadId(int tid) { m_threadID = tid; }

protected:
    void importBooks();
    void importQuran();

signals:
    void bookImported(QString title);
    void BookImportError(QString title);
    void doneImporting();

protected:
    ShamelaInfo *m_shamelaInfo;
    ShamelaManager *m_shamelaManager;
    LibraryCreator m_creator;
    bool m_stop;
    bool m_importQuran;
    int m_threadID;
};

#endif // SHAMELAIMPORTTHREAD_H
