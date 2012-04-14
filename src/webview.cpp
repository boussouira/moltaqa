#include "webview.h"
#include "webpage.h"
#include "utils.h"
#include <qevent.h>

WebView::WebView(QWidget *parent) :
    QWebView(parent)
{
    setPage(new WebPage(this));
    m_frame = page()->mainFrame();

    m_stopScrolling = false;
    m_scrollToBottom = false;
    m_scrollTime.start();

    m_animation = new QPropertyAnimation(m_frame, "scrollPosition", this);
    connect(m_frame, SIGNAL(contentsSizeChanged(QSize)), m_animation, SLOT(stop()));
    connect(m_frame, SIGNAL(javaScriptWindowObjectCleared()),
            SLOT(populateJavaScriptWindowObject()));
}

void WebView::scrollToAya(int pSoraNumber, int pAyaNumber)
{
    QWebElement aya = m_frame->findFirstElement(QString("span#s%1a%2")
                                                .arg(pSoraNumber).arg(pAyaNumber));
    if(!aya.isNull()) {
        // First we unhighlight the highlighted AYA
        m_frame->findFirstElement("span.highlighted").removeClass("highlighted");

        aya.addClass("highlighted");

        // Get the postion of the selected AYA
        QRect highElement = aya.geometry();
        // m_frame heihgt
        int frameHeihgt = m_frame->geometry().height() / 2;
        // The height that should be added to center the selected aya
        int addHeight = highElement.height() / 2 ;
        // it must be less than frameHeight
        while (frameHeihgt < addHeight )
            addHeight = addHeight / 2;
        // The aya position equal ((ayaHeight - frameHeight) + addHeight)
        int ayaPosition = (highElement.y() - frameHeihgt) + addHeight;

        // Animation the scrolling to the selected AYA
        scrollToPosition(QPoint(0, ayaPosition));
    }
}

void WebView::scrollToSora(int soraNumber)
{
    scrollToAya(soraNumber, 1);
}

void WebView::pageDown()
{
    int ypos = m_frame->scrollPosition().y();
    int xpos = m_frame->scrollPosition().x();
    ypos += m_frame->geometry().height();
    ypos -= m_frame->geometry().height()/10;

    ypos = qMin(m_frame->scrollBarMaximum(Qt::Vertical), ypos);

    scrollToPosition(QPoint(xpos, ypos));
}

void WebView::pageUp()
{
    int ypos = m_frame->scrollPosition().y();
    int xpos = m_frame->scrollPosition().x();
    ypos -= m_frame->geometry().height();
    ypos += m_frame->geometry().height()/10;

    ypos = qMax(m_frame->scrollBarMinimum(Qt::Vertical), ypos);

    scrollToPosition(QPoint(xpos, ypos));
}

void WebView::scrollToPosition(const QPoint &pos, int duration)
{
    bool scroll = true;
    if(m_animation->state() == QPropertyAnimation::Running) {
        m_animation->stop();
        scroll = false;
    }

    if(scroll || !m_stopScrolling){
        m_animation->setDuration(duration);
        m_animation->setStartValue(m_frame->scrollPosition());
        m_animation->setEndValue(pos);

        m_animation->start();
    }

    emit textChanged();
}

bool WebView::maxDown()
{
    return m_frame->scrollBarMaximum(Qt::Vertical) <= m_frame->scrollPosition().y()+10;
}

bool WebView::maxUp()
{
    return m_frame->scrollBarMinimum(Qt::Vertical) >= m_frame->scrollPosition().y()-10;
}

void WebView::setStopScroll(bool stopScroll)
{
    m_stopScrolling = stopScroll;
}

void WebView::scrollToBottom(bool scroll)
{
    m_scrollToBottom = scroll;
}

QVariant WebView::execJS(const QString &js)
{
    return m_frame->evaluateJavaScript(js);
}

void WebView::addObject(const QString &name, QObject *object)
{
    m_frame->addToJavaScriptWindowObject(name, object);
}

QString WebView::toHtml()
{
    return m_frame->toHtml();
}

void WebView::setText(const QString &text)
{
    setHtml(text);
    emit textChanged();
}

void WebView::scrollToElement(QString elementQuery)
{
    QWebElement element = m_frame->findFirstElement(elementQuery);
    if(!element.isNull()) {
        QRect highElement = element.geometry();
        // m_frame heihgt
        int frameHeihgt = m_frame->geometry().height() / 2;
        // The height that should be added to center the selected aya
        int addHeight = highElement.height() / 2 ;
        // it must be less than frameHeight
        while (frameHeihgt < addHeight )
            addHeight = addHeight / 2;
        // The aya position equal ((ayaHeight - frameHeight) + addHeight)
        int ayaPosition = (highElement.y() - frameHeihgt) + addHeight;

        // Animation the scrolling to the selected AYA
        scrollToPosition(QPoint(0, ayaPosition));
    }
}

void WebView::pageTextChanged()
{
    if(m_scrollToBottom) {
        m_frame->setScrollPosition(QPoint(m_frame->scrollPosition().x(),
                                          m_frame->scrollBarMaximum(Qt::Vertical)));
        m_scrollToBottom = false;
    }
}

void WebView::scrollToSearch()
{
    scrollToElement(".resultHL");
}

void WebView::openMoltaqaLink(QString link)
{
    QUrl url(link);
    ML_ASSERT(url.scheme() == "moltaqa");

    m_shemeHandler.open(url);
}

void WebView::populateJavaScriptWindowObject()
{
    addObject("webView", this);
}

void WebView::wheelEvent(QWheelEvent *event)
{
    if(m_animation->state() == QPropertyAnimation::Running)
        m_animation->stop();

    bool scrollDown = event->delta() < 0;

    if (event->orientation() == Qt::Vertical) {
        if(scrollDown) {
            if(maxDown() && m_scrollTime.elapsed() > 300) {
                m_scrollToBottom = false;
                emit nextPage();
            }
        } else {
            if(maxUp() && m_scrollTime.elapsed() > 300) {
                m_scrollToBottom = true;
                emit prevPage();
            }
        }

        m_scrollTime.restart();
    }

    QWebView::wheelEvent(event);
}
