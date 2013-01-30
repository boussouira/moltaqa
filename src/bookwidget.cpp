#include "bookwidget.h"
#include "indexwidget.h"
#include "abstractbookreader.h"
#include "webview.h"
#include "textformatter.h"
#include "richsimplebookreader.h"
#include "richquranreader.h"
#include "richtafessirreader.h"
#include "mainwindow.h"
#include "bookreaderhelper.h"
#include "modelenums.h"
#include "modelutils.h"
#include "librarybookmanager.h"
#include "bookreaderview.h"
#include "utils.h"
#include "stringutils.h"

#include <qsplitter.h>
#include <qboxlayout.h>
#include <qtconcurrentrun.h>
#include <qfuturewatcher.h>
#include <qevent.h>
#include <qfile.h>
#include <qplaintextedit.h>
#include <qtreeview.h>
#include <qaction.h>
#include <qtimer.h>

BookWidget::BookWidget(LibraryBook::Ptr book, QWidget *parent):
    BookViewBase(book, parent),
    m_reader(0)
{
    openReader();

    m_layout = new QVBoxLayout(this);
    m_splitter = new QSplitter(this);
    m_view = new WebView(this);
    m_view->setStopScroll(!m_book->isQuran());
    m_view->setBook(m_book);

    m_indexWidget = new IndexWidget(m_splitter);
    m_indexWidget->setBookInfo(m_book);
    m_indexWidget->setCurrentPage(m_reader->page());

    m_bookManager = LibraryManager::instance()->bookManager();
    m_bookHelper = MW->readerHelper();

    m_splitter->addWidget(m_indexWidget);
    m_splitter->addWidget(m_view);
    m_layout->addWidget(m_splitter);
    m_layout->setMargin(0);

    setLayout(m_layout);
    setAutoFillBackground(true);

    loadSettings();
    loadIndexModel();

    m_viewInitialized = false;
    m_indexReading = false;

    connect(m_indexWidget, SIGNAL(openPage(int)), this, SLOT(openPage(int)));
    connect(m_indexWidget, SIGNAL(openSora(int,int)), SLOT(openSora(int,int)));
    connect(m_view->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), SLOT(viewObjectCleared()));
    connect(m_reader, SIGNAL(textChanged()), SLOT(readerTextChanged()));
    connect(m_reader, SIGNAL(textChanged()), SIGNAL(textChanged()));
    connect(m_reader, SIGNAL(textChanged()), m_indexWidget, SLOT(displayBookInfo()));
    connect(m_view->page()->action(QWebPage::Reload), SIGNAL(triggered()), SLOT(reloadCurrentPage()));
    connect(&m_watcher, SIGNAL(finished()), SLOT(indexModelReady()));

    if(!m_book->isQuran()) {
        connect(m_view, SIGNAL(nextPage()), SLOT(wheelNextPage()));
        connect(m_view, SIGNAL(prevPage()), SLOT(wheelPrevPage()));
    }

    setFocusPolicy(Qt::StrongFocus);

    m_view->installEventFilter(this);
    m_indexWidget->treeView()->installEventFilter(this);
}

BookWidget::~BookWidget()
{
    if(m_watcher.isRunning()) {
        m_reader->stopModelLoad();
        m_watcher.waitForFinished();
    }

    ml_delete_check(m_reader);
}

AbstractBookReader *BookWidget::bookReader()
{
    return m_reader;
}

WebViewSearcher *BookWidget::viewSearcher()
{
    return m_view->searcher();
}

void BookWidget::loadSettings()
{
    QByteArray sizes = Utils::Settings::get("BookWidget/splitter").toByteArray();
    if(sizes.size())
        m_splitter->restoreState(sizes);
}

void BookWidget::saveSettings()
{
    Utils::Settings::set("BookWidget/splitter", m_splitter->saveState());
}

void BookWidget::openReader()
{
    if(m_book->isQuran())
        m_reader = new RichQuranReader();
    else if(m_book->isNormal())
        m_reader = new RichSimpleBookReader();
    else if(m_book->isTafessir())
        m_reader = new RichTafessirReader();
    else
        throw BookException(tr("لم يتم التعرف على نوع الكتاب"), QString("Book Type: %1").arg(m_book->type));

    m_reader->setBookInfo(m_book);

    m_reader->openBook();
}

bool BookWidget::search(const QString &text)
{
    return m_view->searcher()->search(text);
}

void BookWidget::searchNext()
{
    m_view->searcher()->next();
}

void BookWidget::searchPrevious()
{
    m_view->searcher()->previous();
}

bool BookWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == m_view || obj == m_indexWidget->treeView()) {
        if(event->type() == QEvent::FocusIn)
            emit gotFocus();
    }

    return false;
}

void BookWidget::loadIndexModel()
{
    QStandardItemModel *model = 0;

    if(m_book->isQuran()) {
        model = m_reader->indexModel();
    } else {
        if(!m_bookHelper->containsBookModel(m_book->id)) {
            m_retModel = QtConcurrent::run(m_reader, &RichBookReader::indexModel);
            m_watcher.setFuture(m_retModel);
        } else {
            QStandardItemModel *savedModel = m_bookHelper->getBookModel(m_book->id);
            if(savedModel)
                model = Utils::Model::cloneModel(savedModel);
        }
    }

    m_indexWidget->setIndex(model ? model : new QStandardItemModel(this));
    m_indexWidget->hideAyaSpin(!m_book->isNormal());
}

void BookWidget::indexModelReady()
{
    QStandardItemModel *model = m_retModel.result();
    ml_return_on_fail(model);

    m_indexWidget->setIndex(model);

    m_indexWidget->displayBookInfo();

    if(!m_bookHelper->containsBookModel(m_book->id))
        m_bookHelper->addBookModel(m_book->id, Utils::Model::cloneModel(model));
}

void BookWidget::openPage(int id)
{
    m_reader->goToPage(id);

    scrollToCurrentAya(true);
}

void BookWidget::firstPage()
{
    m_reader->firstPage();

    scrollToCurrentAya(true);
}

void BookWidget::lastPage()
{
    if(m_book->isQuran()) {
        m_reader->goToSora(112, 1);
        scrollToCurrentAya(true);
    } else {
        m_reader->lastPage();
    }
}

void BookWidget::nextPage()
{
    if(m_reader->hasNext()) {
       m_reader->nextPage();
       scrollToCurrentAya(true);
    }
}

void BookWidget::prevPage()
{
    if(m_reader->hasPrev()) {
        m_reader->prevPage();
        scrollToCurrentAya(true);
    }
}

void BookWidget::wheelNextPage()
{
    if(!m_indexReading)
        nextPage();
}

void BookWidget::wheelPrevPage()
{
    if(!m_indexReading)
        prevPage();
}

void BookWidget::scrollDown()
{
    if(m_book->isQuran()) {
        int page = m_reader->page()->page;

        m_reader->nextAya();
        scrollToCurrentAya(m_reader->page()->page != page);

        m_indexWidget->displayBookInfo();
    } else {
        if(!m_view->maxDown())
            m_view->pageDown();
        else
            nextPage();
    }
}

void BookWidget::scrollUp()
{
    if(m_book->isQuran()) {
        int page = m_reader->page()->page;

        m_reader->prevAya();
        scrollToCurrentAya(m_reader->page()->page != page);

        m_indexWidget->displayBookInfo();
    } else {
        if(!m_view->maxUp()) {
            m_view->pageUp();
        } else {
            m_view->scrollToBottom(true);
            prevPage();
        }
    }
}

void BookWidget::openPage(int pageNum, int partNum)
{
    m_reader->goToPage(pageNum, partNum);

    scrollToCurrentAya(true);
}

void BookWidget::openSora(int sora, int aya)
{
    if(m_book->isQuran() || m_book->isTafessir()) {
        m_reader->goToSora(sora, aya);
        scrollToCurrentAya(true);
    }
}

void BookWidget::openHaddit(int hadditNum)
{
    if(!m_book->isQuran())
        m_reader->goToHaddit(hadditNum);
}

void BookWidget::scrollToCurrentAya(bool timer)
{
    ml_return_on_fail(m_book->isQuran());

    if(timer)
        QTimer::singleShot(500, this, SLOT(scrollToCurrentAya()));
    else
        m_view->scrollToAya(m_reader->page()->sora, m_reader->page()->aya);
}

void BookWidget::hideIndexWidget()
{
    QList<int> sizes;

    if(m_splitter->sizes().at(0) == 0){
        if(m_splitterState.isEmpty()) {
            sizes << 300 << 1000;

            m_splitter->setSizes(sizes);
        } else {
            m_splitter->restoreState(m_splitterState);
        }
    } else {
        sizes << 0 << 10;

        m_splitterState = m_splitter->saveState();
        m_splitter->setSizes(sizes);
    }
}

void BookWidget::readerTextChanged()
{
    QString js;
    if(m_book->shorooh.isEmpty()) {
        js = "setShorooh(false);";
    } else {

        js = "setShorooh([";

        foreach(BookShorooh shareeh, m_book->shorooh) {
            LibraryBook::Ptr book = m_bookManager->getLibraryBook(shareeh.bookID);
            if(!book)
                continue;

            js += "{";
            js += QString("'bookName' : '%1', ").arg(Utils::Html::jsEscape(book->title));
            js += QString("'bookID' : '%1', ").arg(shareeh.bookID);
            js += QString("'pageID' : '%1'").arg(shareeh.pageID);
            js += "},";
        }

        js += "]);";
    }

    if(m_viewInitialized) {
        m_view->execJS(QString("setPageText('%1', %2, %3)")
                       .arg(Utils::Html::jsEscape(m_reader->page()->text))
                       .arg(m_reader->page()->page)
                       .arg(m_reader->page()->part));

        m_view->execJS(js);
    } else {
        m_view->setHtml(m_reader->textFormat()->getHtmlView(m_reader->page()->text, js));
        m_viewInitialized = true;

        if(m_reader->scrollToHighlight())
            QTimer::singleShot(800, m_view, SLOT(scrollToSearch()));
    }

    m_indexReading = false;
}

void BookWidget::reloadCurrentPage()
{
    m_reader->goToPage(m_reader->page()->pageID);
}

void BookWidget::showIndex()
{
    QStandardItemModel *model = m_indexWidget->indexModel();

    HtmlHelper helper;
    helper.beginDiv(".bookIndex");

    for(int i=0; i<model->rowCount(); i++) {
        QStandardItem *item = model->item(i);
        int tid = item->data(ItemRole::idRole).toInt();
        helper.insertHtmlTag("a", item->text(), "", QString("href='#' tid='%1' id='t%1'").arg(tid));
        helper.insertImage("qrc:/images/arrow-left.png", "", QString("tid='%1'").arg(tid));
        helper.insertBr();
    }

    helper.endDiv();

    m_indexReading = true;

    m_view->execJS(QString("setPageText('%1', '', '')").arg(Utils::Html::jsEscape(helper.html())));
}

void BookWidget::showIndex(int tid)
{
    QModelIndex index = Utils::Model::findModelIndex(m_indexWidget->indexModel(), tid);
    ml_return_on_fail(index.isValid());

    QModelIndex child = index.child(0, 0);

    if(child.isValid()) {
        HtmlHelper helper;
        helper.beginDiv(".bookIndex");

        while(child.isValid()) {
            int tid = child.data(ItemRole::idRole).toInt();
            QString text = child.data(Qt::DisplayRole).toString();

            helper.insertHtmlTag("a", text, "", QString("href='#' tid='%1' id='t%1'").arg(tid));
            helper.insertImage("qrc:/images/arrow-left.png", "", QString("tid='%1'").arg(tid));
            helper.insertBr();

            child = child.sibling(child.row()+1, 0);
        }

        helper.endDiv();

        m_reader->page()->titleID = tid;

        m_indexReading = true;

        m_view->execJS(QString("setPageText('%1', '', '')").arg(Utils::Html::jsEscape(helper.html())));
        m_indexWidget->selectTitle(tid);
    } else {
        m_reader->goToPage(tid);
    }
}

QString BookWidget::getBreadcrumbs()
{
    ml_return_val_on_fail(m_indexWidget->indexModel(), QString());

    QList<QPair<int, QString> > list;
    QModelIndex index = Utils::Model::findModelIndex(m_indexWidget->indexModel(), m_reader->page()->titleID);

    while (index.isValid()) {
        list.append(qMakePair(index.data(ItemRole::idRole).toInt(), index.data().toString()));

        index = index.parent();
    }

    HtmlHelper h;
    for(int i=list.size()-1; i>=0; i--) {
        h.insertHtmlTag("span", list.at(i).second, "", QString("tid='%1'").arg(list.at(i).first));

        if(i)
            h.append(" > ");
    }

    return h.html();
}

void BookWidget::viewObjectCleared()
{
    m_view->addObject("bookWidget", this);
    m_view->addObject("bookReaderView", MW->bookReaderView());
}
