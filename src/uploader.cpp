#include "uploader.h"
#include "utils.h"

#include <qdebug.h>
#include <qmessagebox.h>
#include <qtextcodec.h>

UpLoader::UpLoader(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    reply = 0;
    upf = 0;
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

    QByteArray boundaryRegular(QString("--"+QString::number(qrand(), 10)).toAscii());
    QByteArray boundary("\r\n--"+boundaryRegular+"\r\n");
    QByteArray boundaryLast("\r\n--"+boundaryRegular+"--\r\n");

    QUrl url(m_uploadUrl);
    QNetworkRequest request(url);
    request.setRawHeader("Host", url.encodedHost());
    request.setRawHeader("Content-Type", QByteArray("multipart/form-data; boundary=").append(boundaryRegular));

    QByteArray mimedata1("--"+boundaryRegular+"\r\n");
    mimedata1.append("Content-Disposition: form-data; name=\"userfile\"; filename=\""+m_filePath.toUtf8()+"\"\r\n");
    mimedata1.append("Content-Type: text/xml\r\n\r\n");

    QByteArray mimedata2(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"user_id\"\r\n\r\n");
    mimedata2.append(App::id().toUtf8());
    mimedata2.append(boundaryLast);

    upf = new UpFile(m_filePath, mimedata1, mimedata2, this);
    if (upf->openFile())
    {
        reply = manager->post(request, upf);
        connect(reply, SIGNAL(uploadProgress(qint64,qint64)), this, SIGNAL(progress(qint64,qint64)));
        connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
        isInProgress = true;
        emit started();
    } else {
        emit finished(true, false, tr("Error: can't open file %1").arg(m_filePath));
    }
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
    if (upf) {
        upf->close();
        delete upf;
        upf = 0;
    }

    disconnect(reply, SIGNAL(uploadProgress(qint64,qint64)), this, SIGNAL(progress(qint64,qint64)));
    disconnect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));

    if (isAborted) {
        emit finished(false, true, QString());
    } else if (reply->error()>0) {
        emit finished(true, false, tr("Network error\nCode: %1\n%2").arg(QString::number(reply->error())).arg(reply->errorString()));
        //qDebug() << reply->errorString();
    } else {
        QString rep = QString::fromUtf8(reply->readAll());
        //qDebug() << rep;

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
