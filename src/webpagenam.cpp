#include "webpagenam.h"
#include "bookmediaeditor.h"
#include "mimeutils.h"
#include "quazip.h"
#include "quazipfile.h"
#include "qurantextformat.h"
#include "stringutils.h"
#include "utils.h"

#include <qbuffer.h>
#include <qfile.h>
#include <qimage.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qpainter.h>
#include <qsettings.h>
#include <qtimer.h>
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
    m_bookMedia = 0;
}

QNetworkReply *WebPageNAM::getFileContent(const QString &fileName)
{
    CustomNetworkReply *reply = new CustomNetworkReply();

    if(m_bookMedia) {
        BookMediaResource *media = m_bookMedia->getMediaByPath(fileName);
        if(media) {
            reply->setHttpStatusCode(200, "OK");
            reply->setContentType(Utils::Mimes::fileTypeFromFileName(fileName).toUtf8());
            reply->setContent(media->data);
//#ifdef DEV_BUILD
//            qDebug() << "WebPageNAM: Get file content from BookMediaEditor:"
//                     << qPrintable(fileName);
//#endif
            return reply;
        }
    }

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
#ifdef DEV_BUILD
                qWarning() << "WebPageNAM::getFileContent: open error:" << file.getZipError()
                           << "File name:" << fileName << "\n"
                           << "Book:" << qPrintable(m_book->path);
#else
                qWarning() << "WebPageNAM::getFileContent File not found:"
                           << fileName << "in:" << m_book->path;
#endif
            }
        } else {
#ifdef DEV_BUILD
            qWarning() << "WebPageNAM::getFileContent: setCurrentFile error:" << zip.getZipError()
                       << "File name:" << fileName
                       << "Book:" << qPrintable(m_book->path);
#else
            qWarning() << "WebPageNAM::getFileContent File open error:"
                       << fileName << "book:" << m_book->path;
#endif
        }
    } else {
        qWarning() << "WebPageNAM::getFileContent book is not set, can't get"
                   << fileName;
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
    if(req.url().scheme() == "book"
            && req.url().toString() != baseUrl()) {

        if(req.url().host() == "quran") {
            QString ayaNumber = req.url().toString();

            int slashOffset = ayaNumber.lastIndexOf('/');
            int len = ayaNumber.lastIndexOf('.')-slashOffset-1;

            bool ok = false;
            int aya = ayaNumber.mid(slashOffset+1, len).toInt(&ok);

            if(!ok) {
                qWarning() << "WebPageNAM::createRequest Can't get aya number from"
                           << req.url().toString();
                aya = 1;
            }

            QByteArray imageData = QuranTextFormat::getAyaNumberImage(aya);

            CustomNetworkReply *reply = new CustomNetworkReply();
            reply->setHttpStatusCode(200, "OK");
            reply->setContentType("image/png");
            reply->setContent(imageData);

//#ifdef DEV_BUILD
//            qDebug() << "WebPageNAM: GET" << qPrintable(req.url().toString())
//                     << "->" << "Aya number" << aya;
//#endif

            return reply;
        } else {

            QString filename = req.url().toString();

            if(filename.startsWith(baseUrl()))
                filename.remove(0, baseUrl().size() + 1); // Add 1 for the '/'
            else if(filename.startsWith(QLatin1String("book://")))
                filename.remove(0, QString("book://").size());

#ifdef DEV_BUILD
            qDebug() << "WebPageNAM: GET" << qPrintable(req.url().toString())
                     << "->" << qPrintable(filename)
                     << "->" << qPrintable(Utils::Mimes::fileTypeFromFileName(filename));
#endif

            return getFileContent(filename);
        }
    }

    return QNetworkAccessManager::createRequest(op, req, outgoingData);
}

void WebPageNAM::setBookMedia(BookMediaEditor *bookMedia)
{
    m_bookMedia = bookMedia;
}
