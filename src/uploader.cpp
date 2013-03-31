#include "uploader.h"
#include "utils.h"

#include <qnetworkreply.h>
#include <qhttpmultipart.h>
#include <qmessagebox.h>

UpLoader::UpLoader(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    reply = 0;
    isAborted = false;
    isInProgress = false;
}
UpLoader::~UpLoader()
{

}
bool UpLoader::aborted()
{
    return isAborted;
}

void UpLoader::startUpload()
{
    isAborted = false;

    m_multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart userIdPart;
    userIdPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"user_id\""));
    userIdPart.setBody(App::id().toUtf8());

    QHttpPart filePart;
    QString conrent = QString("form-data; name=\"userfile\"; filename=\"%1\"").arg(QFileInfo(m_filePath).fileName());
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, conrent.toUtf8());
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/xml"));

    QFile *file = new QFile(m_filePath);
    file->open(QIODevice::ReadOnly);
    filePart.setBodyDevice(file);
    file->setParent(m_multiPart); // we cannot delete the file now, so delete it with the multiPart

    m_multiPart->append(filePart);
    m_multiPart->append(userIdPart);

    QUrl url(m_uploadUrl);
    QNetworkRequest request(url);

    reply = manager->post(request, m_multiPart);

    connect(reply, SIGNAL(uploadProgress(qint64,qint64)), this, SIGNAL(progress(qint64,qint64)));
    connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));

    isInProgress = true;

    emit started();
}

void UpLoader::abort()
{
    isAborted = true;
    if (reply && isInProgress)
        reply->abort();
}

void UpLoader::replyFinished()
{
    isInProgress = false;

    ml_delete_check (m_multiPart);

    disconnect(reply, SIGNAL(uploadProgress(qint64,qint64)), this, SIGNAL(progress(qint64,qint64)));
    disconnect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));

    if (isAborted) {
        emit finished(false, true, QString());
    } else if (reply->error()>0) {
        emit finished(true, false, tr("Network error\nCode: %1\n%2")
                      .arg(QString::number(reply->error()))
                      .arg(reply->errorString()));

        Utils::Settings::set("Statistics/last",
                             QDateTime::currentDateTime().toTime_t());

#ifdef DEV_BUILD
        qDebug() << "UpLoader::replyFinished Network replay error:"
                 << reply->errorString();
#endif
    } else {
        QString rep = QString::fromUtf8(reply->readAll());
#ifdef DEV_BUILD
        qDebug() << "UpLoader::replyFinished Server replay:"
                 << "\n>>>>>>\n"
                 << rep
                 << "\n<<<<<<";
#endif
        if (rep.contains("{{ok}}", Qt::CaseInsensitive)) {
            emit finished(false, false, QString());
            emit uploadSuccess();
        } else {
            emit finished(true, false, reply->errorString());
        }
    }
}

void UpLoader::setFilePath(QString path)
{
    m_filePath = path;
}

void UpLoader::setUploadUrl(QString url)
{
    m_uploadUrl = url;
}
