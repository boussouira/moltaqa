#include "webview.h"
#include "webpage.h"
#include "utils.h"
#include "stringutils.h"
#include "librarymanager.h"
#include "librarybookmanager.h"
#include "mainwindow.h"
#include "bookwidget.h"
#include "richbookreader.h"

#include <qevent.h>
#include <qmenu.h>
#include <qdesktopservices.h>

WebView::WebView(QWidget *parent) :
    QWebView(parent)
{
    m_page = new WebPage(this);

    setPage(m_page);
    m_frame = page()->mainFrame();

    pageAction(QWebPage::Copy)->setShortcut(QKeySequence(QKeySequence::Copy));

    m_stopScrolling = false;
    m_scrollToBottom = false;
    m_scrollTime.start();

    m_animation = new QPropertyAnimation(m_frame, "scrollPosition", this);
    connect(m_frame, SIGNAL(contentsSizeChanged(QSize)), m_animation, SLOT(stop()));
    connect(m_frame, SIGNAL(javaScriptWindowObjectCleared()),
            SLOT(populateJavaScriptWindowObject()));
}

WebView::~WebView()
{
    ml_delete_check(m_page);
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
    ml_return_on_fail(url.scheme() == "moltaqa");

    m_shemeHandler.open(url);
}

void WebView::populateJavaScriptWindowObject()
{
    addObject("webView", this);
}

void WebView::openLinkInBrowser()
{
    QWebHitTestResult r = page()->mainFrame()->hitTestContent(m_lastMenuPos);
    if (!r.linkUrl().isEmpty()) {
        QDesktopServices::openUrl(r.linkUrl());
    }
}

void WebView::searchInCurrentBook()
{
    ml_return_on_fail2(parent(), "WebView::searchInCurrentBook parent is null");

    BookWidget *p = qobject_cast<BookWidget*>(parent()->parent()); // QSplitter > BookWidget
    ml_return_on_fail2(p, "WebView::searchInCurrentBook book widget is null");

    QString text = selectedText().simplified();
    ml_return_on_fail2(text.size(), "WebView::searchInLibrary search text is empty");

    SearchWidget *searchWidget = MW->searchView()->newTab(SearchWidget::BookSearch,
                                                          p->bookReader()->bookInfo()->id);
    ml_return_on_fail2(searchWidget, "WebView::searchInLibrary searchWidget is null");

    searchWidget->setSearchText(text);
    searchWidget->search();

    MW->showSearchView();
}

void WebView::searchInQuran()
{
    LibraryBookPtr quranBook = LibraryManager::instance()->bookManager()->getQuranBook();
    ml_return_on_fail2(quranBook, "WebView::searchInQuran can't find quran book");

    QString text = selectedText().simplified();
    ml_return_on_fail2(text.size(), "WebView::searchInQuran search text is empty");

    SearchWidget *searchWidget = MW->searchView()->newTab(SearchWidget::BookSearch, quranBook->id);
    ml_return_on_fail2(searchWidget, "WebView::searchInQuran searchWidget is null");

    searchWidget->setSearchText(text);
    searchWidget->search();

    MW->showSearchView();
}

void WebView::searchInLibrary()
{
    QString text = selectedText().simplified();
    ml_return_on_fail2(text.size(), "WebView::searchInLibrary search text is empty");

    SearchWidget *searchWidget = MW->searchView()->newTab(SearchWidget::LibrarySearch);
    ml_return_on_fail2(searchWidget, "WebView::searchInLibrary searchWidget is null");

    searchWidget->setSearchText(text);
    searchWidget->search();

    MW->showSearchView();
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

void WebView::contextMenuEvent(QContextMenuEvent *event)
{
    m_lastMenuPos = event->pos();
    QMenu menu(this);

    QWebHitTestResult r = page()->mainFrame()->hitTestContent(event->pos());
    if (!r.linkUrl().isEmpty()) {
        if(r.linkUrl().scheme() != "moltaqa") {
            menu.addAction(tr("فتح الرابط في المتصفح"), this, SLOT(openLinkInBrowser()));
            menu.addSeparator();
        }

        menu.addAction(pageAction(QWebPage::CopyLinkToClipboard));
    }

    if(selectedText().size() && parent()) {
//        QString text = Utils::String::Arabic::removeTashekil(selectedText().simplified());
//        text = Utils::String::abbreviate(text, 120);
//        QMenu *sub = menu.addMenu(tr("بحث عن '%1' في").arg(text));

        QMenu *sub = menu.addMenu(tr("بحث عن النص المحدد في"));
        BookWidget *p = qobject_cast<BookWidget*>(parent()->parent()); // QSplitter > BookWidget
        if(p)
            sub->addAction(tr("الكتاب الحالي"), this, SLOT(searchInCurrentBook()));

        if(LibraryManager::instance()->bookManager()->getQuranBook())
            sub->addAction(tr("القرآن الكريم"), this, SLOT(searchInQuran()));

        sub->addAction(tr("كل الكتب"), this, SLOT(searchInLibrary()));
        menu.addSeparator();

        menu.addAction(pageAction(QWebPage::Copy));
    }

    if(menu.actions().size()) {
        menu.exec(mapToGlobal(event->pos()));
        return;
    }

    QWebView::contextMenuEvent(event);
}
