#include "mdbconvertermanager.h"
#include "qdebug.h"
#include "qfile.h"

MdbConverterManager::MdbConverterManager()
{
}

MdbConverterManager::~MdbConverterManager()
{
    QHashIterator<QString, QPair<QString, int> > i(m_convertedFiles);
    while (i.hasNext()) {
        i.next();
        qDebug() << "Remove:" << i.value().first;
        QFile::remove(i.value().first);
    }
}

QString MdbConverterManager::getConvertedDB(QString path)
{
    QString mdbPath = path.toLower();
    QPair<QString, int> ref = m_convertedFiles.value(mdbPath, qMakePair(QString(), 0));

    if(ref.first.isEmpty()) {
        MdbConverter m_converter;
        QString convereted = m_converter.exportFromMdb(path);
        m_convertedFiles.insert(mdbPath, qMakePair(convereted, 1));
        return convereted;
    } else {
        qDebug("Coverted Book exist");
        m_convertedFiles[path].second++;
        return m_convertedFiles[path].first;
    }
}

void MdbConverterManager::deleteDB(QString path)
{
    QString mdbPath = path.toLower();
    QPair<QString, int> ref = m_convertedFiles.value(mdbPath, qMakePair(QString(), 0));
    if(!path.isEmpty() && !ref.first.isEmpty()) {
        m_convertedFiles[mdbPath].second--;
        if(m_convertedFiles[mdbPath].second <= 0) {
            m_convertedFiles.remove(mdbPath);
            QFile::remove(m_convertedFiles[mdbPath].first);
            qDebug() << "Delete converted book" << m_convertedFiles[mdbPath].first;
        } else {
            qDebug() << "Book is used" << m_convertedFiles[mdbPath].first;
        }
    } else {
        qDebug() << "Delete" << m_convertedFiles[mdbPath].first << "which doesn't exist";
    }
}
