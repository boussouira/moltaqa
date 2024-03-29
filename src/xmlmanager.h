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

    XmlDomHelper &domHelper() { return m_dom; }

protected:
    virtual void saveModel(QXmlStreamWriter &writer, QStandardItemModel *model)=0;
    void makeBackup();

protected:
    XmlDomHelper m_dom;
};

#endif // XMLMANAGER_H
