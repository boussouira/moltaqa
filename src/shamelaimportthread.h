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
    void importCats();
    void importAuthors();
    void importBooks();

signals:
    void setStepsRange(int minVal, int maxVal);
    void setStep(int value);
    void setRange(int minValue, int maxValue);
    void setProgress(int value);
    void stepTitle(QString title);
    void debugInfo(QString text);
    void doneImporting();

public: //TODO: make this private
    bool m_stop;
    ShamelaInfo *m_shamela;
    ShamelaManager *m_manager;
    LibraryInfo *m_library;
    LibraryCreator m_creator;
};

#endif // SHAMELAIMPORTTHREAD_H
