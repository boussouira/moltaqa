#ifndef XMLMANAGER_H
#define XMLMANAGER_H

#include "listmanager.h"
#include "xmldomhelper.h"

class QStandardItemModel;
class QXmlStreamWriter;

class XmlManager : public ListManager
{
    Q_OBJECT
public:
    XmlManager(QObject *parent = 0);
    
    void setFilePath(QString path);
    void save(QStandardItemModel *model);

    void reloadModels();

protected:
    virtual void saveModel(QXmlStreamWriter &writer, QStandardItemModel *model);

protected:
    XmlDomHelper m_dom;
};

#endif // XMLMANAGER_H
