#include "serverapi.h"
#include "utils.h"

#define API_SERVER "http://localhost/moltaqa-lib/api/"
#define API_MAKE_URL(p) (API_SERVER#p)

#define MAP_STR(map, key) map.value(key).toString()
#define MAP_BOOL(map, key) map.value(key).toBool()

ServerApi::ServerApi(QObject *parent) : QObject(parent)
{
    m_user = 0;
    m_cookieJar = new QNetworkCookieJar();
    m_nam = new QNetworkAccessManager();

    m_nam->setCookieJar(m_cookieJar);
    m_cookieJar->setParent(0);
}

void ServerApi::login(QString username, QString password, bool remember)
{
    QUrl url(API_MAKE_URL(user/login));
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QUrl postData;
    postData.addQueryItem("identity", username);
    postData.addQueryItem("password", password);

    if(remember)
        postData.addQueryItem("remember", "true");

    QNetworkReply *replay = m_nam->post(req, postData.encodedQuery());
    connect(replay, SIGNAL(finished()), SLOT(loginReplay()));
}

void ServerApi::loginReplay()
{
    QNetworkReply *replay = qobject_cast<QNetworkReply*>(sender());
//    if(replay->attribute(QNetworkRequest::RedirectionTargetAttribute).isValid()) {
//        QUrl url = replay->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
//        DEBUG("NOT HANDLED: Redirect to:" << url.toString());
//    }

    if(replay->error()) {
        qDebug() << "Replay erro:" << replay->errorString();
        return;
    }

    QString replayText = QString::fromUtf8(replay->readAll());
//    qDebug() << replayText;
    Json::JsonObject replayJson(replayText);
    if (replayJson.getBool("stat")) {
        qDebug("Logged in: %s", qPrintable(replayJson.getString("message")));

        ml_delete_check(m_user);

        m_user = new User(replayJson.getObject("user"));
        qDebug() << "Welcome" << qPrintable(m_user->getUserName());
    } else {
        qDebug("Not logged in: %s", qPrintable(replayJson.getString("message")));
    }

}

QNetworkCookieJar *ServerApi::cookieJar() const
{
    return m_cookieJar;
}
