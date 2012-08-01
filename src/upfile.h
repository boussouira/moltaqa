#ifndef QUPFILE_H
#define QUPFILE_H

#include <qfile.h>
#include <qiodevice.h>

class UpFile : public QIODevice
{
    Q_OBJECT
public:
    UpFile(const QString & name, const QByteArray &head, const QByteArray &tail, QObject *parent = 0);
    ~UpFile();
    bool openFile();
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 size() const;

private:
    QFile *file;
    QByteArray *head;
    QByteArray *tail;
    qint64 position;
    qint64 sizepart() const;
    qint64 sizefull() const;
};

#endif // QUPFILE_H
