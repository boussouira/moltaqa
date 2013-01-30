#ifndef BOOKVIEWPDF_H
#define BOOKVIEWPDF_H

#include "bookviewbase.h"
#include <qdatetime.h>

class WebView;
class QVBoxLayout;

namespace Poppler {
class Document;
}

class BookViewPdf : public BookViewBase
{
    Q_OBJECT
public:
    BookViewPdf(LibraryBook::Ptr book, QWidget *parent = 0);

    AbstractBookReader *bookReader();
    WebViewSearcher *viewSearcher();

    bool maxDown();
    bool maxUp();

public slots:
    void firstPage();
    void lastPage();
    void nextPage();
    void prevPage();

    void openPage(int id){}
    void openPage(int pageNum, int partNum){}
    void openSora(int sora, int aya){}
    void openHaddit(int hadditNum){}

    void scrollDown(){}
    void scrollUp(){}

    void reloadCurrentPage(){}

protected slots:
    void resizePage();

protected:
    void getPage(int pid);
    void wheelEvent(QWheelEvent *event);

protected:
    QVBoxLayout *m_layout;
    class QLabel *m_imageLabel;
    class QScrollArea *m_scrollArea;

    int m_currentPage;

    Poppler::Document *m_doc;
    qreal m_zoom;
    int m_dpiX;
    int m_dpiY;

    QTime m_scrollTime;
    bool m_scrollToBottom;
};

#endif // BOOKVIEWPDF_H
