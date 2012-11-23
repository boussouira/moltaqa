#include "xmlmanager.h"
#include "xmlutils.h"
#include "utils.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qdebug.h>
#include <qxmlstream.h>

XmlManager::XmlManager(QObject *parent) :
    ListManager(parent)
{
}

void XmlManager::setFilePath(QString path)
{
    m_dom.setFilePath(path);
}

void XmlManager::save(QStandardItemModel *model)
{
    if(m_dom.needSave()) {
        qCritical("XmlManager::save The XML need to be save before saving the model");
        m_dom.setNeedSave(false);
    }

    makeBackup();

    QFile file(m_dom.filePath());

    ml_return_on_fail2(file.open(QIODevice::WriteOnly), "ListManager::save open file error" << file.errorString());

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);

    saveModel(writer, model);

    file.close();
    reloadModels();
}

void XmlManager::reloadModels()
{
    m_dom.reload();

    ListManager::reloadModels();
}

void XmlManager::makeBackup()
{
    QFileInfo info(m_dom.filePath());
    QDir dir(info.dir());

    if(!dir.exists("backup"))
        dir.mkdir("backup");

    dir.cd("backup");

    QFileInfoList backups = dir.entryInfoList(QStringList() << info.baseName()+"*",
                                              QDir::Files|QDir::NoDotAndDotDot,
                                              QDir::Time);
    if(backups.size() > 5) {
        for(int i=5; i < backups.size(); i++) {
            QFile::remove(backups.at(i).filePath());
        }
    }

    QString fileName = QString("%1_%2.%3")
            .arg(info.baseName())
            .arg(QDateTime::currentDateTime().toString("dd-MM-yyyy_hh-mm-ss"))
            .arg(info.completeSuffix());

    ml_warn_on_fail(QFile::copy(m_dom.filePath(), dir.filePath(fileName)),
                    "XmlManager::makeBackup Can't copy file" << m_dom.filePath() << "to" << dir.filePath(fileName));
}
