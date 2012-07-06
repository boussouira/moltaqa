#include "bookwidget.h"
#include "indexwidget.h"
#include "abstractbookreader.h"
#include "webview.h"
#include "textformatter.h"
#include "richsimplebookreader.h"
#include "mainwindow.h"
#include "bookreaderhelper.h"
#include "modelenums.h"
#include "modelutils.h"
#include "librarybookmanager.h"
#include "booksviewer.h"
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

BookWidget::BookWidget(RichBookReader *db, QWidget *parent): QWidget(parent), m_db(db)
{
    m_layout = new QVBoxLayout(this);
    m_splitter = new QSplitter(this);
    m_view = new WebView(this);
    m_view->setStopScroll(!m_db->bookInfo()->isQuran());

    m_indexWidget = new IndexWidget(m_splitter);
    m_indexWidget->setBookInfo(db->bookInfo());
    m_indexWidget->setCurrentPage(db->page());

    m_bookManager = LibraryManager::instance()->bookManager();

    m_splitter->addWidget(m_indexWidget);
    m_splitter->addWidget(m_view);
    m_layout->addWidget(m_splitter);
    m_layout->setMargin(0);

    setLayout(m_layout);
    setAutoFillBackground(true);

    loadSettings();
    displayInfo();

    m_viewInitialized = false;
    m_indexReading = false;

    connect(m_indexWidget, SIGNAL(openPage(int)), this, SLOT(openPage(int)));
    connect(m_indexWidget, SIGNAL(openSora(int,int)), SLOT(openSora(int,int)));
    connect(m_view->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), SLOT(viewObjectCleared()));
    connect(m_db, SIGNAL(textChanged()), SLOT(readerTextChanged()));
    connect(m_db, SIGNAL(textChanged()), SIGNAL(textChanged()));
    connect(m_db, SIGNAL(textChanged()), m_indexWidget, SLOT(displayBookInfo()));
    connect(m_view->page()->action(QWebPage::Reload), SIGNAL(triggered()), SLOT(reloadCurrentPage()));

    if(!m_db->bookInfo()->isQuran()) {
        connect(m_view, SIGNAL(nextPage()), SLOT(wheelNextPage()));
        connect(m_view, SIGNAL(prevPage()), SLOT(wheelPrevPage()));
    }

    setFocusPolicy(Qt::StrongFocus);

    m_view->installEventFilter(this);
    m_indexWidget->treeView()->installEventFilter(this);
}

BookWidget::~BookWidget()
{
    delete m_db;
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

void BookWidget::focusInEvent(QFocusEvent *event)
{
    if(event->gotFocus())
        emit gotFocus();
}

void BookWidget::displayInfo()
{
    m_indexWidget->setIndex(m_db->indexModel());

    m_indexWidget->hideAyaSpin(!m_db->bookInfo()->isNormal());
}

void BookWidget::openPage(int id)
{
    m_db->goToPage(id);

    scrollToCurrentAya(true);
}

void BookWidget::firstPage()
{
    m_db->firstPage();

    scrollToCurrentAya(true);
}

void BookWidget::lastPage()
{
    if(m_db->bookInfo()->isQuran()) {
        m_db->goToSora(112, 1);
        scrollToCurrentAya(true);
    } else {
        m_db->lastPage();
    }
}

void BookWidget::nextPage()
{
    if(bookReader()->hasNext()) {
       m_db->nextPage();
       scrollToCurrentAya(true);
    }
}

void BookWidget::prevPage()
{
    if(bookReader()->hasPrev()) {
        m_db->prevPage();
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
    if(m_db->bookInfo()->isQuran()) {
        int page = m_db->page()->page;

        m_db->nextAya();
        scrollToCurrentAya(m_db->page()->page != page);

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
    if(m_db->bookInfo()->isQuran()) {
        int page = m_db->page()->page;

        m_db->prevAya();
        scrollToCurrentAya(m_db->page()->page != page);

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
    m_db->goToPage(pageNum, partNum);

    scrollToCurrentAya(true);
}

void BookWidget::openSora(int sora, int aya)
{
    if(m_db->bookInfo()->isQuran() || m_db->bookInfo()->isTafessir()) {
        m_db->goToSora(sora, aya);
        scrollToCurrentAya(true);
    }
}

void BookWidget::openHaddit(int hadditNum)
{
    if(!m_db->bookInfo()->isQuran())
        m_db->goToHaddit(hadditNum);
}

void BookWidget::scrollToCurrentAya(bool timer)
{
    ml_return_on_fail(m_db->bookInfo()->isQuran());

    if(timer)
        QTimer::singleShot(500, this, SLOT(scrollToCurrentAya()));
    else
        m_view->scrollToAya(m_db->page()->sora, m_db->page()->aya);
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
    if(m_db->bookInfo()->shorooh.isEmpty()) {
        js = "setShorooh(false);";
    } else {

        js = "setShorooh([";

        foreach(BookShorooh shareeh, m_db->bookInfo()->shorooh) {
            LibraryBookPtr book = m_bookManager->getLibraryBook(shareeh.bookID);
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
                       .arg(Utils::Html::jsEscape(m_db->page()->text))
                       .arg(m_db->page()->page)
                       .arg(m_db->page()->part));

        m_view->execJS(js);
    } else {
        m_view->setHtml(m_db->textFormat()->getHtmlView(m_db->page()->text, js));
        m_viewInitialized = true;

        if(m_db->scrollToHighlight())
            QTimer::singleShot(800, m_view, SLOT(scrollToSearch()));
    }

    m_indexReading = false;
}

void BookWidget::reloadCurrentPage()
{
    m_db->goToPage(m_db->page()->pageID);
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

        m_db->page()->titleID = tid;

        m_indexReading = true;

        m_view->execJS(QString("setPageText('%1', '', '')").arg(Utils::Html::jsEscape(helper.html())));
        m_indexWidget->selectTitle(tid);
    } else {
        m_db->goToPage(tid);
    }
}

QString BookWidget::getBreadcrumbs()
{
    QList<QPair<int, QString> > list;
    QModelIndex index = Utils::Model::findModelIndex(m_indexWidget->indexModel(), m_db->page()->titleID);

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
    m_view->addObject("booksViewer", MW->booksViewer());
}
