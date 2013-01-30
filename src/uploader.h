#ifndef UPLOADER_H
#define UPLOADER_H

#include <qobject.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkrequest.h>
#include <qnetworkreply.h>
#include <qurl.h>

#include "upfile.h"

class UpLoader : public QObject
{
    Q_OBJECT

public:
    UpLoader(QObject *parent = 0);
    ~UpLoader();
    bool aborted();
    void setFilePath(QString path);
    void setUploadUrl(QString url);

public slots:
    void startUpload();
    void abort();

private slots:
    void replyFinished();

signals:
    void started();
    void progress(qint64 bytesSent, qint64 bytesTotal);
    void finished(bool error, bool aborted, const QString &text);
    void uploadSuccess();

private:
    QHttpMultiPart *m_multiPart;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QString m_uploadUrl;
    QString m_filePath;
    bool isAborted;
    bool isInProgress;
};

#endif // UPLOADER_H
