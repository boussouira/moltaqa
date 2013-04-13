#ifndef LISTMANAGER_H
#define LISTMANAGER_H

#include <qmutex.h>
#include <qobject.h>

class ListManager : public QObject
{
    Q_OBJECT
public:
    ListManager(QObject *parent = 0);
    ~ListManager();

    virtual void loadModels()=0;
    virtual void clear()=0;
    virtual void reloadModels();

signals:
    void ModelsReady();

protected:
    QMutex m_mutex;
};

#endif // LISTMANAGER_H
