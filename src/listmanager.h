#ifndef LISTMANAGER_H
#define LISTMANAGER_H

#include <qobject.h>
#include <qdom.h>
#include <qmutex.h>
#include "xmldomhelper.h"

class QStandardItemModel;
class QXmlStreamWriter;

class ListManager : public QObject
{
    Q_OBJECT
public:
    ListManager(QObject *parent = 0);
    ~ListManager();

    void setFilePath(QString path);

    virtual void loadModels()=0;
    virtual void clear();

    void reloadModels();

    void save(QStandardItemModel *model);

protected:
    virtual void saveModel(QXmlStreamWriter &writer, QStandardItemModel *model);

signals:
    void ModelsReady();

protected:
    QMutex m_mutex;
    bool m_domLoaded;
    bool m_saveDom;
    XmlDomHelper m_dom;
};

#endif // LISTMANAGER_H
