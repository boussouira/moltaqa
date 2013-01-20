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
#include <qclipboard.h>
#include <qapplication.h>

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

    m_searcher = new WebViewSearcher(this);

    m_animation = new QPropertyAnimation(m_frame, "scrollPosition", this);
    connect(m_page, SIGNAL(openMoltaqaLink(QString)), SLOT(openMoltaqaLink(QString)));
    connect(m_frame, SIGNAL(contentsSizeChanged(QSize)), m_animation, SLOT(stop()));
    connect(m_frame, SIGNAL(javaScriptWindowObjectCleared()),
            SLOT(populateJavaScriptWindowObject()));
}

WebView::~WebView()
{
    ml_delete_check(m_page);
    ml_delete_check(m_searcher);
}

void WebView::setBook(LibraryBook::Ptr book)
{
    m_book = book;
    m_page->setBook(book);
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

    if(duration == -1)
        duration = qMin(500, qAbs(pos.y()-m_frame->scrollPosition().y())/2);

    if(scroll || !m_stopScrolling){
        m_animation->setEasingCurve(QEasingCurve::OutQuad);
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

LibraryBook::Ptr WebView::getLibraryBook()
{
    ml_return_val_on_fail2(parent(), "WebView::getLibraryBook parent is null", LibraryBook::Ptr());

    BookWidget *p;
    QObject *parentObj = parent();
    while (parentObj) {
        p = qobject_cast<BookWidget*>(parentObj);
        if(p)
            break;

        parentObj = parentObj->parent();
    }

//    p = qobject_cast<BookWidget*>(parent()->parent()); // QSplitter > BookWidget
    ml_return_val_on_fail2(p, "WebView::getLibraryBook book widget is null", LibraryBook::Ptr());

    return p->bookReader()->book();
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
    ml_return_on_fail2(m_book, "WebView::searchInCurrentBook book is null");

    QString text = selectedText().simplified();
    ml_return_on_fail2(text.size(), "WebView::searchInLibrary search text is empty");

    SearchWidget *searchWidget = MW->searchView()->newTab(SearchWidget::BookSearch,
                                                          m_book->id);
    ml_return_on_fail2(searchWidget, "WebView::searchInLibrary searchWidget is null");

    searchWidget->setSearchText(text);
    searchWidget->search();

    MW->showSearchView();
}

void WebView::searchInQuran()
{
    LibraryBook::Ptr quranBook = LibraryManager::instance()->bookManager()->getQuranBook();
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

void WebView::copyWithRefer()
{
    QAction *act = qobject_cast<QAction*>(sender());
    ml_return_on_fail(act);

    ml_return_on_fail2(parent(), "WebView::copyWithRefer parent is null");

    BookWidget *bookWidget = qobject_cast<BookWidget*>(parent()->parent());
    ml_return_on_fail2(bookWidget, "WebView::copyWithRefer book widget is null");

    RichBookReader *reader = bookWidget->bookReader();

    QString referText = act->data().toString();
    referText.replace(_u("*النص*"),   selectedText().trimmed());
    referText.replace(_u("*المؤلف*"), reader->book()->authorName);
    referText.replace(_u("*الكتاب*"), reader->book()->title);
    referText.replace(_u("*الصفحة*"), QString::number(reader->page()->page));
    referText.replace(_u("*الجزء*"),  QString::number(reader->page()->part));
    referText.replace(_u("*الحديث*"), QString::number(reader->page()->haddit));

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(referText);
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

    if(selectedText().trimmed().size()) {
        QMenu *searchMenu = menu.addMenu(tr("بحث عن النص المحدد في"));

        if(m_book)
            searchMenu->addAction(tr("الكتاب الحالي"), this, SLOT(searchInCurrentBook()));

        if(LibraryManager::instance()->bookManager()->getQuranBook())
            searchMenu->addAction(tr("القرآن الكريم"), this, SLOT(searchInQuran()));

        searchMenu->addAction(tr("كل الكتب"), this, SLOT(searchInLibrary()));
        menu.addSeparator();

        if(m_book && !m_book->isQuran()) {
            QMenu *referMenu = menu.addMenu(tr("نسخ النص مع العزو"));
            foreach(QAction *act, LibraryManager::instance()->textRefersActions()) {
                referMenu->addAction(act);
                connect(act, SIGNAL(triggered()), SLOT(copyWithRefer()));
            }

            menu.addSeparator();
        }

        menu.addAction(pageAction(QWebPage::Copy));
    }

    if(menu.actions().size()) {
        menu.exec(mapToGlobal(event->pos()));
        return;
    }

    QWebView::contextMenuEvent(event);
}

void WebView::keyPressEvent(QKeyEvent *event)
{
    if((event->modifiers() & Qt::ControlModifier)) {
        qreal zoom = zoomFactor();
        if(event->key() == Qt::Key_Plus)
            zoom += 0.1;
        else if(event->key() == Qt::Key_Minus)
            zoom -= 0.1;
        else if(event->key() == Qt::Key_0)
            zoom = 1.0;

        if(zoom != zoomFactor() && 0.1 < zoom && zoom < 5.0) {
            setZoomFactor(zoom);
        }
    }

    QWebView::keyPressEvent(event);
}
