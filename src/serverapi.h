#ifndef SERVERAPI_H
#define SERVERAPI_H

#include <QtNetwork>
#include "json/json.h"

class ServerApi : public QObject
{
    Q_OBJECT

    /* Temporary classes */

public:
    class User {
    public:
        User(Json::JsonObject data) : m_json(data){}

        QString getUserName() { return m_json.getString("username"); }

    protected:
        Json::JsonObject m_json;
    };

    /* Temporary classes */

public:
    ServerApi(QObject *parent = 0);

    void login(QString user, QString password, bool remember);
    void logout();

    QNetworkCookieJar *cookieJar() const;

protected slots:
    void loginReplay();

private:
    QNetworkCookieJar *m_cookieJar;
    QNetworkAccessManager *m_nam;
    User *m_user;
};

#endif // SERVERAPI_H
