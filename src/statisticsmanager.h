#ifndef STATISTICSMANAGER_H
#define STATISTICSMANAGER_H

#include "xmlmanager.h"

#include <qhash.h>

class UpLoader;

class StatisticsManager : public XmlManager
{
    Q_OBJECT

public:
    StatisticsManager(QObject *parent = 0);
    ~StatisticsManager();
    
    static StatisticsManager *instance();

    void loadModels();
    void clear();

    void add(QString type, QString data, bool printDebug=true);

    void enqueue(QString type, QString data, bool printDebug=true);
    void dequeue(QString type, QString data, bool printDebug=true);

protected:
    virtual void saveModel(QXmlStreamWriter &, QStandardItemModel *) {}
    void printStatistics(QString type, QString data);

protected slots:
    void maySendStatistics();
    void sendStatistics();
    void sendSuccess();

protected:
    QHash<QString, QString> m_pending;
    UpLoader *m_uploader;
};

#endif // STATISTICSMANAGER_H
