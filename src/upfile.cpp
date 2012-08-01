#include "upfile.h"

UpFile::UpFile(const QString &name, const QByteArray &head, const QByteArray &tail, QObject *parent) : QIODevice(parent)
{
    file = new QFile(name, this);
    this->head = new QByteArray(head);
    this->tail = new QByteArray(tail);
    position = 0;
}
UpFile::~UpFile()
{
    delete head;
    delete tail;
}

//////////////////////////////pure virtual//////////////////////////////////////////
qint64 UpFile::readData(char *data, qint64 maxlen)
{
    if (!file->isOpen())
    {
        return -1;
    }
    char *pointer = data;
    qint64 atAll = 0;
    if ((position<head->size()) && (maxlen>0))
    {
        qint64 count = qMin(maxlen, (qint64)head->size());
        memcpy(pointer, head->data(), count);
        pointer += count;
        position+=count;
        atAll+=count;
        maxlen -= count;
    }
    if (position>=sizepart() && (maxlen>0) && (position<sizefull()))
    {
        qint64 count = qMin(maxlen, (qint64)tail->size());
        memcpy(pointer, tail->data(), count);
        position+=count;
        atAll+=count;
    }
    if (maxlen>0 && (position<sizefull()))
    {
        qint64 count = qMin(maxlen, file->bytesAvailable());
        int s = file->read(pointer, count);
        pointer+=s;
        maxlen -= s;
        position+=s;
        atAll+=s;
    }
    return atAll;
}

qint64 UpFile::writeData(const char *, qint64)
{
    return -1;
}

qint64 UpFile::size() const
{
    return sizefull();
}

/////////////////////////////////public//////////////////////////////////
bool UpFile::openFile()
{
    if (file->open(QIODevice::ReadOnly))
        return this->open(QIODevice::ReadOnly);
    else {
        qDebug("Can`t open %s", qPrintable(file->fileName()));
        return false;
    }
}
//////////////////////////////private////////////////////////////////////////////
qint64 UpFile::sizefull() const
{
    return file->size()+head->size()+tail->size();
}

qint64 UpFile::sizepart() const
{
    return head->size()+file->size();
}
