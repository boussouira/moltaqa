#ifndef KWEBVIEW_H
#define KWEBVIEW_H

#include <qwebview.h>
#include <qwebframe.h>
#include <qwebelement.h>
#include <qpropertyanimation.h>
#include "shemehandler.h"
#include <QTime>

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

    void setStopScroll(bool stopScroll);
    void scrollToBottom(bool scroll);

    QVariant execJS(const QString &js);
    void addObject(const QString &name, QObject *object);
    QString toHtml();

protected:
    void wheelEvent(QWheelEvent *event);

public slots:
    void setText(const QString &text);
    void scrollToElement(QString elementQuery);
    void pageTextChanged();
    void scrollToSearch();
    void openMoltaqaLink(QString link);

protected slots:
    void populateJavaScriptWindowObject();

signals:
    void textChanged();
    void nextPage();
    void prevPage();

protected:
    QWebFrame *m_frame;
    QPropertyAnimation *m_animation;
    ShemeHandler m_shemeHandler;
    QTime m_scrollTime;
    bool m_scrollToBottom;
    bool m_stopScrolling;
};

#endif // KWEBVIEW_H
