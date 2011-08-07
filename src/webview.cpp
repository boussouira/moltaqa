#include "webview.h"

WebView::WebView(QWidget *parent) : QWebView(parent)
{
    m_frame = page()->mainFrame();

    m_animation = new QPropertyAnimation(m_frame, "scrollPosition", this);
    connect(m_frame, SIGNAL(contentsSizeChanged(QSize)), m_animation, SLOT(stop()));
}

void WebView::scrollToAya(int pSoraNumber, int pAyaNumber)
{
    // First we unhighlight the highlighted AYA
    m_frame->findFirstElement("span.highlighted").removeClass("highlighted");

    // Since each AYA has it own uniq id, we can highlight
    // any AYA in the current page by adding the class "highlighted"
    m_frame->findFirstElement(QString("span#s%1a%2")
                            .arg(pSoraNumber).arg(pAyaNumber)).addClass("highlighted");

    // Get the postion of the selected AYA
    QRect highElement = m_frame->findFirstElement("span.highlighted").geometry();
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

    scrollToPosition(QPoint(xpos, ypos));
}

void WebView::pageUp()
{
    int ypos = m_frame->scrollPosition().y();
    int xpos = m_frame->scrollPosition().x();
    ypos -= m_frame->geometry().height();
    ypos += m_frame->geometry().height()/10;

    scrollToPosition(QPoint(xpos, ypos));
}

void WebView::scrollToPosition(const QPoint &pos, int duration)
{
    if(m_animation->state() == QPropertyAnimation::Running) {
        m_animation->stop();
    } else {
        m_animation->setDuration(duration);
        m_animation->setStartValue(m_frame->scrollPosition());
        m_animation->setEndValue(pos);

        m_animation->start();
    }

    emit textChanged();
}

bool WebView::maxDown()
{
    return (page()->mainFrame()->scrollBarMaximum(Qt::Vertical)==
            page()->mainFrame()->scrollPosition().y());
}

bool WebView::maxUp()
{
    return (page()->mainFrame()->scrollBarMinimum(Qt::Vertical)==
            page()->mainFrame()->scrollPosition().y());
}

void WebView::setText(const QString &text)
{
    setHtml(text);
    emit textChanged();
}