#ifndef KWEBVIEW_H
#define KWEBVIEW_H

#include <QtWebKit>
#include <QPropertyAnimation>

class KWebView : public QWebView
{
public:
    KWebView(QWidget* parent = 0);
    void scrollToAya(int pSoraNumber, int pAyaNumber);
    void scrollToSora(int soraNumber);
    void pageDown();
    void pageUp();
    void scrollToPosition(const QPoint &pos, int duration=1000);
    bool maxDown();
    bool maxUp();

};

#endif // KWEBVIEW_H
