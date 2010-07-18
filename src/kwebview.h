#ifndef KWEBVIEW_H
#define KWEBVIEW_H

#include <qwebview.h>
#include <qwebframe.h>
#include <qwebelement.h>

#if QT_VERSION >= 0x040600
    #include <qpropertyanimation.h>
#else
    #include <qtimeline.h>
#endif

class KWebView : public QWebView
{
    Q_OBJECT
public:
    KWebView(QWidget* parent = 0);
    void scrollToAya(int pSoraNumber, int pAyaNumber);
    void scrollToSora(int soraNumber);
    void pageDown();
    void pageUp();
    void scrollToPosition(const QPoint &pos, int duration=1000);
    bool maxDown();
    bool maxUp();

public slots:
    void setY(int y);

protected:
    QWebFrame *m_frame;
#if QT_VERSION >= 0x040600
    QPropertyAnimation *m_animation;
#else
    QTimeLine *m_timeLine;
#endif
};

#endif // KWEBVIEW_H
