#include "bookviewpdf.h"
#include "webview.h"
#include "richsimplebookreader.h"
#include "utils.h"

#include <QVBoxLayout>
#include <qprocess.h>
#include <qapplication.h>
#include <qdesktopwidget.h>

#include <poppler/qt4/poppler-qt4.h>
#include <qlabel.h>
#include <qscrollarea.h>
#include <qscrollbar.h>
#include <qtimer.h>
#include <QMouseEvent>

BookViewPdf::BookViewPdf(LibraryBook::Ptr book, QWidget *parent) :
    BookViewBase(book, parent)
{
    /*
    m_view = new WebView(this);
    m_view->setBook(m_book);
    
    QUrl url = QUrl::fromLocalFile("/home/naruto/Programming/pdf.js/web/viewer.html");
    url.addQueryItem("file", m_book->path);
    m_view->setUrl(url);
    
    m_layout = new QVBoxLayout(this);
    m_layout->addWidget(m_view);
    m_layout->setMargin(0);
    
    setLayout(m_layout);
    */
    
    m_zoom = 1.0;
    m_dpiX = 72.0;
    m_dpiY = 72.0;
    
    m_currentPage = 0;
    
    m_imageLabel = new QLabel(this);
    
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setBackgroundRole(QPalette::Dark);
    m_scrollArea->setAlignment(Qt::AlignHCenter);
    m_scrollArea->setWidget(m_imageLabel);
    
    m_layout = new QVBoxLayout(this);
    m_layout->addWidget(m_scrollArea);
    m_layout->setMargin(0);
    setLayout(m_layout);
    
    m_doc = Poppler::Document::load(m_book->path);
    if(m_doc == NULL) {
        throw BookException(tr("لا يمكن فتح الكتاب: %1").arg(book->title), book->path);
    }
    
    m_doc->setRenderHint(Poppler::Document::TextAntialiasing, true);
    m_doc->setRenderHint(Poppler::Document::Antialiasing, true);
    m_doc->setRenderBackend(Poppler::Document::SplashBackend);
    
    getPage(m_currentPage);
    
    //    QTimer::singleShot(0, this, SLOT(resizePage()));
}

AbstractBookReader *BookViewPdf::bookReader()
{
    return 0;
}

WebViewSearcher *BookViewPdf::viewSearcher()
{
    return 0;
}

void BookViewPdf::firstPage()
{
    getPage(0);
}

void BookViewPdf::lastPage()
{
    getPage(m_doc->numPages()-1);
}

void BookViewPdf::nextPage()
{
    getPage(++m_currentPage);
}

void BookViewPdf::prevPage()
{
    if(m_currentPage - 1 >= 0)
        getPage(--m_currentPage);
}

bool BookViewPdf::maxDown()
{
    return m_scrollArea->verticalScrollBar()->maximum() <= m_scrollArea->verticalScrollBar()->value()+10;
}

bool BookViewPdf::maxUp()
{
    return m_scrollArea->verticalScrollBar()->maximum() >= m_scrollArea->verticalScrollBar()->value()-10;
}

void BookViewPdf::resizePage()
{
    // resX -> label.width
    // x    -> scroll.width
    
    // x = (resX * scroll.width) / label.width
    
    //    qDebug("BE ScrollArea: %d, View: %d, Label: %d, SBar: %d", m_scrollArea->contentsRect().width(), width(), m_imageLabel->width(), m_scrollArea->verticalScrollBar()->width());
    //    m_scrollArea->verticalScrollBar()->hide();
    //    qDebug("AF ScrollArea: %d, View: %d, Label: %d, SBar: %d", m_scrollArea->contentsRect().width(), width(), m_imageLabel->width(), m_scrollArea->verticalScrollBar()->width());
    // -m_scrollArea->verticalScrollBar()->width()
    double dpiX = (((double)m_dpiX * (double)(m_scrollArea->contentsRect().width())) / (double)m_imageLabel->width());
    double rot = (double)dpiX / (double)m_dpiX;
    
    //    qDebug() << "Rot" << rot;
    //    m_dpiX = dpiX;
    
    m_dpiX *= rot;
    m_dpiY *= rot;
    
    getPage(m_currentPage);
}

void BookViewPdf::getPage(int pid)
{
    Poppler::Page *popplerPage = m_doc->page(pid);
    ml_return_on_fail2(popplerPage, "BookViewPdf::getPage Can't get page" << pid);
    const double resX = m_dpiX * m_zoom;
    const double resY = m_dpiY * m_zoom;
    QImage image = popplerPage->renderToImage(resX, resY);
    if (!image.isNull()) {
        m_imageLabel->resize(image.size());
        m_imageLabel->setPixmap(QPixmap::fromImage(image));
        
        m_currentPage = pid;
    } else {
        m_imageLabel->resize(0, 0);
        m_imageLabel->setPixmap(QPixmap());
    }
    
    delete popplerPage;
    
    m_scrollArea->verticalScrollBar()->setValue(0);
    
    /*
    QString program = "/usr/bin/gs";
    QStringList arguments;
    arguments << "-dBATCH"
              << "-dNOPAUSE"
              << "-sDEVICE=pngalpha "
              << "-r144"
              << "-dFirstPage=" + QString::number(pid)
              << "-dLastPage=" + QString::number(pid)
              << "-sOutputFile=" + QString("/home/naruto/xml-lib/temp/cover.png")
              << m_book->path;
              
    QProcess *myProcess = new QProcess(this);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    myProcess->setProcessEnvironment(env);
    
    myProcess->start(program, arguments);
    myProcess->waitForFinished();
    qDebug() << QString::fromUtf8(myProcess->readAll());
    qDebug() << myProcess->processEnvironment().toStringList().join("\n");
    qDebug() << arguments.join(" ");
    qDebug() << myProcess->errorString();
    
            QUrl url = QUrl::fromLocalFile("home/naruto/xml-lib/temp/cover.jpeg");
    m_view->setUrl(url);
    */
}

void BookViewPdf::wheelEvent(QWheelEvent *event)
{
    //    if(m_scrollTime.elapsed() < 150)
    //        return;
    
    qDebug() << m_scrollTime.elapsed();
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
    
    BookViewBase::wheelEvent(event);
}
