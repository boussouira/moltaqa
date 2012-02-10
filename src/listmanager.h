#ifndef LISTMANAGER_H
#define LISTMANAGER_H

#include <QObject>
#include <qdom.h>
#include <qmutex.h>

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

protected:
    QMutex m_mutex;
    QString m_filePath;
    QDomDocument m_doc;
    QDomElement m_rootElement;
    bool m_domLoaded;
    bool m_saveDom;
};

#endif // LISTMANAGER_H
