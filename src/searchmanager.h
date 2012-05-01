#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

#include "databasemanager.h"

class SearchFieldInfo {
public:
    SearchFieldInfo() {}
    QString name;
    int fieldID;
};

class SearchManager : public DatabaseManager
{
    Q_OBJECT

public:
    SearchManager(QObject *parent = 0);
    
    void loadModels();
    void clear();
    
    QList<SearchFieldInfo> getFieldNames();
    bool addField(const QString &name, const QList<int> &list);
    QList<int> getFieldBooks(int fid);

    void removeField(int fid);
    void setFieldName(const QString &name, int fid);
};

#endif // SEARCHMANAGER_H
