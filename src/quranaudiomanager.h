#ifndef QURANAUDIOMANAGER_H
#define QURANAUDIOMANAGER_H

#include "xmlmanager.h"

class QuranAudioManager : public XmlManager
{
    Q_OBJECT
public:
    QuranAudioManager(QObject *parent = 0);
    ~QuranAudioManager();

    void loadModels();
    void clear();
    
    QStandardItemModel* recitersModel();

protected:
    void saveModel(QXmlStreamWriter &writer, QStandardItemModel *model);
    QStandardItemModel* getModel();

protected:
    QStandardItemModel *m_model;
};

#endif // QURANAUDIOMANAGER_H
