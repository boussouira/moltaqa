#ifndef KWEBVIEW_H
#define KWEBVIEW_H

#include <qwebview.h>
#include <qwebframe.h>
#include <qwebelement.h>
#include <qpropertyanimation.h>
#include <qdatetime.h>
#include <qnetworkreply.h>
#include <qfile.h>

#include "shemehandler.h"
#include "webviewsearcher.h"
#include "librarybook.h"

class WebPage;
class WebPageNAM;
class QWebFrame;
class QPropertyAnimation;
class WebViewSearcher;

class WebView : public QWebView
{
    Q_OBJECT
public:
    WebView(QWidget* parent = 0);
    ~WebView();

    void setBook(LibraryBook::Ptr book);

    void scrollToAya(int pSoraNumber, int pAyaNumber);
    void scrollToSora(int soraNumber);
    bool maxDown();
    bool maxUp();

    void setStopScroll(bool stopScroll);
    void scrollToBottom(bool scroll);

    QVariant execJS(const QString &js);
    void addObject(const QString &name, QObject *object);
    void autoObjectAdd(QString objectName, QObject *object);

    QString toHtml();

    WebViewSearcher *searcher();
    LibraryBook::Ptr getLibraryBook();

    WebPageNAM *getPageNAM();

protected:
    void wheelEvent(QWheelEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
    void keyPressEvent(QKeyEvent *event);

public slots:
    void setText(const QString &text);
    void scrollToElement(QString elementQuery, bool center=true);
    void scrollToPosition(const QPoint &pos, int duration=1000);
    void pageDown();
    void pageUp();
    void pageTextChanged();
    void scrollToSearch();
    void openMoltaqaLink(QString link);

protected slots:
    void populateJavaScriptWindowObject();
    void openLinkInBrowser();
    void searchInCurrentBook();
    void searchInQuran();
    void searchInLibrary();
    void copyWithRefer();
    void copyHtml();

    void downloadRequested(const QNetworkRequest &request);
    void downloadingFileFinished();
    void downloadingError(QNetworkReply::NetworkError code);
    void httpReadyRead();

signals:
    void textChanged();
    void nextPage();
    void prevPage();

protected:
    WebPage *m_page;
    QWebFrame *m_frame;
    QPropertyAnimation *m_animation;
    ShemeHandler m_shemeHandler;
    QTime m_scrollTime;
    QPoint m_lastMenuPos;
    bool m_scrollToBottom;
    bool m_stopScrolling;
    LibraryBook::Ptr m_book;
    WebViewSearcher *m_searcher;
    QFile m_downloadFile;
    QHash<QString, QObject*> m_autoObjectAdd;
};

#endif // KWEBVIEW_H
