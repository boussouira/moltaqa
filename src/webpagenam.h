#ifndef WEBPAGENAM_H
#define WEBPAGENAM_H

#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include "librarybook.h"

class CustomNetworkReply : public QNetworkReply
{
    Q_OBJECT

public:
    CustomNetworkReply(QObject *parent=0);
    ~CustomNetworkReply();

    void setHttpStatusCode(int code, const QByteArray &statusText = QByteArray());
    void setHeader(QNetworkRequest::KnownHeaders header, const QVariant &value);
    void setContentType(const QByteArray &contentType);

    void setContent(const QString &content);
    void setContent(const QByteArray &content);

    void abort();
    qint64 bytesAvailable() const;
    bool isSequential() const;

protected:
    qint64 readData(char *data, qint64 maxSize);

private:
    struct QCustomNetworkReplyPrivate *d;
};

class WebPageNAM : public QNetworkAccessManager
{
public:
    WebPageNAM(QObject *parent = 0);

    void setBook(LibraryBook::Ptr book) { m_book = book; }
    QNetworkReply *getFileContent(const QString &fileName);

protected:
    QNetworkReply *createRequest(Operation op, const QNetworkRequest &req, QIODevice *outgoingData = 0);

    LibraryBook::Ptr m_book;
};



#endif // WEBPAGENAM_H
