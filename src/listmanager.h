#ifndef LISTMANAGER_H
#define LISTMANAGER_H

#include <qobject.h>
#include <qdom.h>
#include <qmutex.h>
#include <qset.h>

class QStandardItemModel;
class QXmlStreamWriter;

class ListManager : public QObject
{
    Q_OBJECT
public:
    ListManager(QObject *parent = 0);
    ~ListManager();

    void setFilePath(QString path);

    void loadXmlDom();
    void saveXmlDom();

    void reloadXmlDom();

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
    QString m_filePath;
    QDomDocument m_doc;
    QDomElement m_rootElement;
    bool m_domLoaded;
    bool m_saveDom;
};

#endif // LISTMANAGER_H
