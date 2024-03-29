#include "updatechecker.h"
#include "utils.h"

#include <qdom.h>
#include <qmessagebox.h>

UpdateChecker::UpdateChecker(QObject *parent) :
    QObject(parent)
{
    hasError = false;
    autoCheck = false;
}

UpdateChecker::~UpdateChecker()
{
    qDeleteAll(m_results);
    m_results.clear();
}

void UpdateChecker::startCheck(bool autoUpdateCheck)
{
    m_result = 0;
    m_replayText.clear();
    autoCheck = autoUpdateCheck;

#ifdef APP_UPDATE_REVISION
    QString updateUrl = Utils::Settings::get("Update/url",
                                            "http://dl.dropbox.com/s/k8wtn3js2bi51uy/update.xml?dl=1").toString();

    if(updateUrl.contains("{ID}"))
        updateUrl.replace("{ID}", App::id());

    updateUrl.replace("{REVISION}", QString::number(APP_UPDATE_REVISION));
    updateUrl.replace("{OS}",
                  #ifdef Q_OS_WIN
                      "windows"
                  #else
                      "linux"
                  #endif
                      );

    startRequest(QUrl(updateUrl));
#else
    qWarning("Can't check for update without Git change number");
#endif
}

UpdateInfo *UpdateChecker::result()
{
    return m_result;
}

void UpdateChecker::startRequest(QUrl url)
{
    m_reply = m_qnam.get(QNetworkRequest(url));
    connect(m_reply, SIGNAL(finished()), SLOT(httpFinished()));
    connect(m_reply, SIGNAL(readyRead()), SLOT(httpReadyRead()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            SLOT(updateError(QNetworkReply::NetworkError)));
}

void UpdateChecker::httpFinished()
{
    QVariant redirectionTarget = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (m_reply->error()) {
        errorString = m_reply->errorString();
        hasError = true;

        emit checkFinished();
    } else if (!redirectionTarget.isNull()) {
#ifdef DEV_BUILD
        qDebug() << "UpdateChecker::httpFinished Redirect from:"
                 << redirectionTarget.toUrl().toString() << "To:"
                 << m_reply->url().toString();
#endif
            QUrl url = m_reply->url().resolved(redirectionTarget.toUrl());
            m_reply->deleteLater();
            m_replayText.clear();

            startRequest(url);
            return;
    } else {
        parse(m_replayText);

        hasError = false;
        emit checkFinished();
    }

    m_reply->deleteLater();
    m_reply = 0;
}

void UpdateChecker::updateError(QNetworkReply::NetworkError)
{
    errorString = m_reply->errorString();
    hasError = true;
}

void UpdateChecker::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    m_replayText.append(_u(m_reply->readAll()));
}

void UpdateChecker::parse(QString updateXML)
{
    QDomDocument doc;
    if(!doc.setContent(updateXML)) {
        qDebug() << "Parse error:" << updateXML ;
        m_result = 0;
        return;
    }

    m_result = new UpdateInfo();
    QDomElement root = doc.documentElement();

    QDomElement e = root.firstChildElement();
    while(!e.isNull()) {
        if(e.nodeName() == "revision") {
            m_result->revision = e.text().toInt();
        } else if(e.nodeName() == "version") {
            m_result->versionStr = e.text();
            m_result->version = e.attribute("num").toInt();
        } else if(e.nodeName() == "download") {
            m_result->downloadLink = e.text();
        } else if(e.nodeName() == "changelog") {
            m_result->changelog = e.text();
        } else if(e.nodeName() == "url") {
            Utils::Settings::set("Update/url", e.text());
        } else if(e.nodeName() == "statistics-send") {
            Utils::Settings::set("Statistics/send", e.text().compare("true", Qt::CaseInsensitive)==0);
        } else if(e.nodeName() == "statistics-url") {
            Utils::Settings::set("Statistics/url", e.text());
        }

        e = e.nextSiblingElement();
    }

    if(m_result->revision <= APP_UPDATE_REVISION) {
        ml_delete_check(m_result);
    } else {
        m_results.append(m_result);
    }
}
