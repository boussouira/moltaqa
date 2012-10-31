#include "webpagenam.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#include "utils.h"
#include "mimeutils.h"

#include <qnetworkrequest.h>
#include <QNetworkAccessManager>
#include <qdebug.h>
#include <qnetworkreply.h>
#include <QTimer>
#include <qfile.h>
#include <qwebelement.h>


struct QCustomNetworkReplyPrivate
{
    QByteArray content;
    qint64 offset;
};

CustomNetworkReply::CustomNetworkReply(QObject *parent)
    : QNetworkReply(parent)
{
    d = new QCustomNetworkReplyPrivate;
}

CustomNetworkReply::~CustomNetworkReply()
{
    delete d;
}

void CustomNetworkReply::setHttpStatusCode(int code, const QByteArray &statusText)
{
    setAttribute(QNetworkRequest::HttpStatusCodeAttribute, code);
    if (statusText.isNull())
        return;

    setAttribute(QNetworkRequest::HttpReasonPhraseAttribute, statusText);
}

void CustomNetworkReply::setHeader(QNetworkRequest::KnownHeaders header, const QVariant &value)
{
    QNetworkReply::setHeader(header, value);
}

void CustomNetworkReply::setContentType(const QByteArray &contentType)
{
    setHeader(QNetworkRequest::ContentTypeHeader, contentType);
}

void CustomNetworkReply::setContent(const QString &content)
{
    setContent(content.toUtf8());
}

void CustomNetworkReply::setContent(const QByteArray &content)
{
    d->content = content;
    d->offset = 0;

    open(ReadOnly | Unbuffered);
    setHeader(QNetworkRequest::ContentLengthHeader, QVariant(content.size()));

    QTimer::singleShot(0, this, SIGNAL(readyRead()));

    if (d->content.isEmpty() || attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 404)
        QTimer::singleShot(0, this, SIGNAL(finished()));
}

void CustomNetworkReply::abort()
{
    // NOOP
}

qint64 CustomNetworkReply::bytesAvailable() const
{
    return d->content.size() - d->offset;
}

bool CustomNetworkReply::isSequential() const
{
    return true;
}


qint64 CustomNetworkReply::readData(char *data, qint64 maxSize)
{
    if (d->offset >= d->content.size())
        return -1;

    qint64 number = qMin(maxSize, d->content.size() - d->offset);
    memcpy(data, d->content.constData() + d->offset, number);
    d->offset += number;

    if (d->offset >= d->content.size())
        QTimer::singleShot(0, this, SIGNAL(finished()));
    else
        QTimer::singleShot(0, this, SIGNAL(readyRead()));

    return number;
}

WebPageNAM::WebPageNAM(QObject *parent) :
    QNetworkAccessManager(parent)
{
}

QNetworkReply *WebPageNAM::getFileContent(const QString &fileName)
{
    //qDebug() << "WebPageNAM::getFileContent" << fileName << "from" << m_book->path;
    CustomNetworkReply *reply = new CustomNetworkReply();

    if(m_book) {
        QuaZip zip;
        zip.setZipName(m_book->path);

        if(!zip.open(QuaZip::mdUnzip)) {
            qCritical() << "WebPageNAM::openZip open book error"
                        << zip.getZipError();
        }

        QuaZipFile file(&zip);

        if(zip.setCurrentFile(fileName)) {
            if(file.open(QIODevice::ReadOnly)) {
                reply->setHttpStatusCode(200, "OK");
                reply->setContentType(Utils::Mimes::fileTypeFromFileName(fileName).toUtf8());

                reply->setContent(file.readAll());

                return reply;
            } else {
                qWarning() << "WebPageNAM::getFileContent: open error:" << file.getZipError()
                           << "\nFile name:" << fileName << "\n"
                           << "\nBook:" << qPrintable(m_book->path);
            }
        } else {
            qWarning() << "WebPageNAM::getFileContent: setCurrentFile error:"<< zip.getZipError()
                       << "\nFile name:" << fileName
                       << "\nBook:" << qPrintable(m_book->path);
        }
    }

    reply->setHttpStatusCode(404, "Not Found");
    reply->setContentType("text/html");

    if(m_book)
        reply->setContent(QString("<p>File %1 not found in book %2</p><p>Path: %3</p>")
                          .arg(fileName).arg(m_book->title).arg(m_book->path));
    else
        reply->setContent(QString("<p>File %1 not found</p>").arg(fileName));

    return reply;

}

QNetworkReply *WebPageNAM::createRequest(Operation op, const QNetworkRequest &req, QIODevice *outgoingData)
{
    if(req.url().toString().startsWith("../")
            || req.url().scheme() == "book") {

//        qDebug() << "WebPageNAM::createRequest handling:" << req.url().toString();

        QString filename = req.url().toString();

        if(filename.startsWith("book://"))
            filename.remove(0, req.url().scheme().size()+3);

        if(filename.startsWith("../"))
            filename.remove(0, 3);

//        qDebug() << filename << "->" << Utils::Mimes::fileTypeFromFileName(filename);

        return getFileContent(filename);
    }

    return QNetworkAccessManager::createRequest(op, req, outgoingData);
}
