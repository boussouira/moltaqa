#ifndef WELCOMEWIDGET_H
#define WELCOMEWIDGET_H

#include "abstarctview.h"

class LibraryBookManager;
class WebView;
class QNetworkCookieJar;

class WelcomeWidget : public AbstarctView
{
    Q_OBJECT

public:
    WelcomeWidget(QWidget *parent = 0);
    ~WelcomeWidget();

    QString title();

    void setNetworkCookieJar(QNetworkCookieJar *cookieJar);

protected:
    void loadSettings();
    void saveSettings();
    void setupHTML();

public slots:
    void open(QString vid);
    QString getFontFamily();
    QString getFontSize();

protected slots:
    void showStatistics();

private:
    LibraryBookManager *m_bookManager;
    WebView *m_webView;
};

#endif // WELCOMEWIDGET_H
