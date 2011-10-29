#ifndef KWEBVIEW_H
#define KWEBVIEW_H

#include <qwebview.h>
#include <qwebframe.h>
#include <qwebelement.h>
#include <qpropertyanimation.h>


class WebView : public QWebView
{
    Q_OBJECT
public:
    WebView(QWidget* parent = 0);
    void scrollToAya(int pSoraNumber, int pAyaNumber);
    void scrollToSora(int soraNumber);
    void pageDown();
    void pageUp();
    void scrollToPosition(const QPoint &pos, int duration=1000);
    bool maxDown();
    bool maxUp();

    QVariant execJS(QString js);
    void addObject(const QString &name, QObject *object);
    QString toHtml();

public slots:
    void setText(const QString &text);

signals:
    void textChanged();

protected:
    QWebFrame *m_frame;
    QPropertyAnimation *m_animation;
};

#endif // KWEBVIEW_H
