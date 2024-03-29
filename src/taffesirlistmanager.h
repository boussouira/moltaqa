#ifndef TAFFESIRLISTMANAGER_H
#define TAFFESIRLISTMANAGER_H

#include "xmlmanager.h"

class QStandardItemModel;

class TaffesirListManager : public XmlManager
{
    Q_OBJECT

public:
    TaffesirListManager(QObject *parent=0);
    ~TaffesirListManager();

    void loadModels();
    void clear();

    QStandardItemModel* taffesirListModel();
    QStandardItemModel* allTaffesirModel();
    void addTafessir(int bookID, QString taffesirName);

protected:
    void saveModel(QXmlStreamWriter &writer, QStandardItemModel *model);
    QStandardItemModel* getModel(bool allTaffasir=false);

protected:
    QStandardItemModel *m_model;
};

#endif // TAFFESIRLISTMANAGER_H
