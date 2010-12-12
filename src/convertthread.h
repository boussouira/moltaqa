#ifndef CONVERTTHREAD_H
#define CONVERTTHREAD_H

#include <qthread.h>
#include <qstringlist.h>

class ImportModel;
class ImportModelNode;
class IndexDB;
class QSqlDatabase;

class ConvertThread : public QThread
{
    Q_OBJECT

public:
    ConvertThread(QObject *parent = 0);
    void setFiles(QStringList &list) { m_files = list;}
    void setModel(ImportModel *model) { m_model = model;}
    void setIndexDB(IndexDB *indexDB) { m_indexDB = indexDB;}
    int convertTime() { return m_convertTime; }
    int convertedFiles() { return m_convertedFiles; }
    void run();

protected:
    void getBookInfo(const QString &path, QList<ImportModelNode*> &nodes);
    QString getBookType(const QSqlDatabase &bookDB);

signals:
    void setProgress(int prog);

protected:
    ImportModel *m_model;
    IndexDB *m_indexDB;
    QStringList m_files;
    int m_convertTime;
    int m_convertedFiles;
};

#endif // CONVERTTHREAD_H
