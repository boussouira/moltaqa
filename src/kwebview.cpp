#include "kwebview.h"

KWebView::KWebView(QWidget *parent) : QWebView(parent)
{
}

void KWebView::scrollToAya(int pSoraNumber, int pAyaNumber)
{

    QWebFrame *frame = page()->mainFrame();

    // First we unhighlight the highlighted AYA
    frame->findFirstElement("span.highlighted").removeClass("highlighted");

    // Since each AYA has it own uniq id, we can highlight
    // any AYA in the current page by adding the class "highlighted"
    frame->findFirstElement(QString("span#s%1a%2")
                            .arg(pSoraNumber).arg(pAyaNumber)).addClass("highlighted");

    // Get the postion of the selected AYA
    QRect highElement = frame->findFirstElement("span.highlighted").geometry();
    // Frame heihgt
    int frameHeihgt = frame->geometry().height() / 2;
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

void KWebView::scrollToSora(int soraNumber)
{
    scrollToAya(soraNumber, 1);
}

void KWebView::pageDown()
{
    QWebFrame *frame = page()->mainFrame();
    int ypos = frame->scrollPosition().y();
    int xpos = frame->scrollPosition().x();
    ypos += frame->geometry().height();
    ypos -= frame->geometry().height()/10;

    scrollToPosition(QPoint(xpos, ypos));
}

void KWebView::pageUp()
{
    QWebFrame *frame = page()->mainFrame();
    int ypos = frame->scrollPosition().y();
    int xpos = frame->scrollPosition().x();
    ypos -= frame->geometry().height();
    ypos += frame->geometry().height()/10;

    scrollToPosition(QPoint(xpos, ypos));
}

void KWebView::scrollToPosition(const QPoint &pos, int duration)
{
    QWebFrame *frame = page()->mainFrame();
    QPropertyAnimation *animation = new QPropertyAnimation(frame, "scrollPosition");
    animation->setDuration(duration);
    animation->setStartValue(frame->scrollPosition());
    animation->setEndValue(pos);

    animation->start();
}

bool KWebView::maxDown()
{
    return (page()->mainFrame()->scrollBarMaximum(Qt::Vertical)==
            page()->mainFrame()->scrollPosition().y());
}

bool KWebView::maxUp()
{
    return (page()->mainFrame()->scrollBarMinimum(Qt::Vertical)==
            page()->mainFrame()->scrollPosition().y());
}
