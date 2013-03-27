#include "statisticsmanager.h"
#include "booklistmanager.h"
#include "mainwindow.h"
#include "librarymanager.h"
#include "libraryinfo.h"
#include "librarybook.h"
#include "utils.h"
#include "xmlutils.h"
#include "timeutils.h"
#include "stringutils.h"
#include "modelenums.h"
#include "authorsmanager.h"
#include "librarybookmanager.h"
#include "uploader.h"

#include <qdir.h>
#include <qstandarditemmodel.h>
#include <qxmlstream.h>
#include <QTimer>

#define UPLOAD_URL "http://localhost/moltaqa-lib/statistics/upload_file/{OS}"
static StatisticsManager *m_instance = 0;

StatisticsManager::StatisticsManager(QObject *parent) :
    XmlManager(parent)
{
    ml_set_instance(m_instance, this);

    QDir dataDir(MW->libraryInfo()->dataDir());
    m_dom.setFilePath(dataDir.filePath("usage.xml"));

    m_uploader = new UpLoader(this);

    if(Utils::Settings::get("Statistics/send", false).toBool())
        QTimer::singleShot(5000, this, SLOT(maySendStatistics()));
}

StatisticsManager::~StatisticsManager()
{
    m_instance = 0;
}

StatisticsManager *StatisticsManager::instance()
{
    return m_instance;
}

void StatisticsManager::loadModels()
{
    m_dom.load();
}

void StatisticsManager::clear()
{
}

void StatisticsManager::add(QString type, QString data, bool printDebug)
{
    ml_return_on_fail2(type.size(), "StatisticsManager::add type is empty");
    ml_return_on_fail2(data.size(), "StatisticsManager::add data is empty");

    QMutexLocker locker(&m_mutex);

    QDomElement titleElement = m_dom.domDocument().createElement("info");
    titleElement.setAttribute("type", type.toLower());
    titleElement.setAttribute("date", QDateTime::currentDateTime().toTime_t());

    titleElement.appendChild(m_dom.domDocument().createCDATASection(data));

    m_dom.rootElement().appendChild(titleElement);

    m_dom.setNeedSave(true);

    if(printDebug)
        printStatistics(type, data);
}

void StatisticsManager::enqueue(QString type, QString data, bool printDebug)
{
    QString saved = m_pending.value(type);

    if(saved.size())
        saved.append(", ");

    saved.append(data);

    m_pending[type] = saved;

    if(printDebug)
        printStatistics(type, data);
}

void StatisticsManager::dequeue(QString type, QString data, bool printDebug)
{
    QString saved = m_pending.value(type);

    if(saved.size())
        saved.append(", ");

    saved.append(data);

    add(type, saved, false);

    if(printDebug)
        printStatistics(type, data);

    m_pending.remove(type);
}

void StatisticsManager::printStatistics(QString type, QString data)
{
    type[0] = type[0].toUpper();
    qDebug() << qPrintable(type + ":") << qPrintable(data);
}

void StatisticsManager::maySendStatistics()
{
    uint current = QDateTime::currentDateTime().toTime_t();
    uint lastSend = Utils::Settings::get("Statistics/last", 0).toUInt();
    if(current - lastSend > 86400) // A day
        sendStatistics();
}

void StatisticsManager::sendStatistics()
{
    if(m_dom.rootElement().elementsByTagName("info").count() > 1) {
        QString url = Utils::Settings::get("Statistics/url", UPLOAD_URL).toString();

        url.replace("{ID}", App::id());
        url.replace("{REVISION}", QString::number(APP_UPDATE_REVISION));
        url.replace("{OS}",
            #ifdef Q_OS_WIN
                    "windows"
            #else
                    "linux"
            #endif
                    );

        m_uploader->setFilePath(m_dom.filePath());
        m_uploader->setUploadUrl(url);
        m_uploader->startUpload();

        connect(m_uploader, SIGNAL(uploadSuccess()), SLOT(sendSuccess()));
    }
}

void StatisticsManager::sendSuccess()
{
    Utils::Settings::set("Statistics/last",
                         QDateTime::currentDateTime().toTime_t());

    QDomNode child = m_dom.rootElement().firstChild();
    while(!child.isNull()) {
        QDomNode nextChild = child.nextSibling();

        m_dom.rootElement().removeChild(child);

        child = nextChild;
    }

    m_dom.setNeedSave(true);
    m_dom.save();
}
