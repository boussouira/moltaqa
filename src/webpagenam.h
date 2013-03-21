#ifndef WEBPAGENAM_H
#define WEBPAGENAM_H

#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include "librarybook.h"

class BookMediaEditor;

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
    Q_OBJECT

public:
    WebPageNAM(QObject *parent = 0);

    void setBook(LibraryBook::Ptr book) { m_book = book; }
    QNetworkReply *getFileContent(const QString &fileName);

    static QString baseUrl() { return "book://data"; }

    void setBookMedia(BookMediaEditor *bookMedia);

protected:
    QNetworkReply *createRequest(Operation op, const QNetworkRequest &req, QIODevice *outgoingData = 0);

    LibraryBook::Ptr m_book;
    BookMediaEditor *m_bookMedia;
};



#endif // WEBPAGENAM_H
