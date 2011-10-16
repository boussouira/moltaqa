#include "bookwidget.h"
#include "indexwidget.h"
#include "abstractbookreader.h"
#include "webview.h"
#include "textformatter.h"
#include "richsimplebookreader.h"

#include <qsplitter.h>
#include <qboxlayout.h>
#include <qtconcurrentrun.h>
#include <qfuturewatcher.h>
#include <QCloseEvent>
#include <QFile>

BookWidget::BookWidget(RichBookReader *db, QWidget *parent): QWidget(parent), m_db(db)
{
    m_layout = new QVBoxLayout(this);
    m_splitter = new QSplitter(this);
    m_view = new WebView(m_splitter);
    m_indexWidget = new IndexWidget(m_splitter);
    m_indexWidget->setBookInfo(db->bookInfo());
    m_indexWidget->setCurrentPage(db->page());

    m_watcher = new QFutureWatcher<QAbstractItemModel*>(this);
    connect(m_watcher, SIGNAL(finished()), SLOT(indexModelReady()));

    m_splitter->addWidget(m_indexWidget);
    m_splitter->addWidget(m_view);
    m_layout->addWidget(m_splitter);
    m_layout->setMargin(0);
    setLayout(m_layout);
    setAutoFillBackground(true);

    loadSettings();
    displayInfo();
    connect(m_indexWidget, SIGNAL(openPage(int)), this, SLOT(openPage(int)));
    connect(m_indexWidget, SIGNAL(openSora(int,int)), SLOT(openSora(int,int)));
    connect(m_db, SIGNAL(textChanged()), SLOT(readerTextChanged()));
    connect(m_db, SIGNAL(textChanged()), m_indexWidget, SLOT(displayBookInfo()));
}

BookWidget::~BookWidget()
{
    if(m_watcher->isRunning()) {
        qDebug("Waiting for model...");
        m_watcher->waitForFinished();
    }

    delete m_db;
}

void BookWidget::loadSettings()
{
    QSettings settings;
    settings.beginGroup("BookWidget");
    QByteArray sizes = settings.value("splitter").toByteArray();
    if(!sizes.isEmpty())
        m_splitter->restoreState(sizes);
    settings.endGroup();
}

void BookWidget::saveSettings()
{
    QSettings settings;
    settings.beginGroup("BookWidget");
    settings.setValue("splitter", m_splitter->saveState());
    settings.endGroup();
}

void BookWidget::setBookReader(RichBookReader *db)
{
    m_db = db;
    m_indexWidget->setBookInfo(db->bookInfo());
    m_indexWidget->setCurrentPage(db->page());

    connect(db, SIGNAL(textChanged()), SLOT(readerTextChanged()));
}

void BookWidget::displayInfo()
{
    if(m_db->needFastIndexLoad()) {
        m_retModel = QtConcurrent::run(m_db, &RichBookReader::indexModel);
        m_watcher->setFuture(m_retModel);

        m_indexWidget->setIndex(m_db->topIndexModel());
    } else {
        m_indexWidget->setIndex(m_db->indexModel());
    }

    m_indexWidget->hideAyaSpin(m_db->bookInfo()->isQuran() || m_db->bookInfo()->isTafessir());
    m_indexWidget->hidePartSpin(m_db->bookInfo()->partsCount > 1);
}

void BookWidget::openPage(int id)
{
    m_db->goToPage(id);

    if(m_db->bookInfo()->isQuran())
        m_view->scrollToSora(id);
}

void BookWidget::firstPage()
{
    m_db->goToPage();

    if(m_db->bookInfo()->isQuran())
        m_view->scrollToSora(1); // First sora
}

void BookWidget::lastPage()
{
    if(m_db->bookInfo()->isQuran()) {
        m_db->goToPage(m_db->bookInfo()->lastPage(), 1);
        m_view->scrollToAya(112, 1);
    } else {
        m_db->goToPage(-2);
    }
}

void BookWidget::nextPage()
{
    if(bookReader()->hasNext()) {
       m_db->nextPage();
        if(m_db->bookInfo()->isQuran())
            m_view->scrollToAya(m_db->page()->sora,
                                m_db->page()->aya);
    }
}

void BookWidget::prevPage()
{
    if(bookReader()->hasPrev()) {
        m_db->prevPage();
        if(m_db->bookInfo()->isQuran())
            m_view->scrollToAya(m_db->page()->sora,
                                m_db->page()->aya);
    }
}

void BookWidget::scrollDown()
{
    if(m_db->bookInfo()->isQuran()) {
        m_db->nextAya();
        m_view->scrollToAya(m_db->page()->sora,
                            m_db->page()->aya);
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
        m_db->prevAya();
        m_view->scrollToAya(m_db->page()->sora,
                            m_db->page()->aya);
    } else {
        if(!m_view->maxUp())
            m_view->pageUp();
        else
            prevPage();
    }
}

void BookWidget::openPage(int pageNum, int partNum)
{
    m_db->goToPage(pageNum, partNum);
    if(m_db->bookInfo()->isQuran())
        m_view->scrollToAya(m_db->page()->sora,
                            m_db->page()->aya);
}

void BookWidget::openSora(int sora, int aya)
{
    if(m_db->bookInfo()->isQuran() || m_db->bookInfo()->isTafessir())
   {     m_db->goToSora(sora, aya);

    if(m_db->bookInfo()->isQuran())
        m_view->scrollToAya(m_db->page()->sora,
                            m_db->page()->aya);
    else if(m_db->bookInfo()->isTafessir())
        m_view->scrollToAya(sora, aya);
}}

void BookWidget::openHaddit(int hadditNum)
{
    if(!m_db->bookInfo()->isQuran())
        m_db->goToHaddit(hadditNum);
}

void BookWidget::hideIndexWidget()
{
    QList<int> sizes;

    if(m_splitter->sizes().at(0) == 0){
        sizes << 300 << 1000;
        m_splitter->setSizes(sizes);
    } else {
        sizes << 0 << 10;
        m_splitter->setSizes(sizes);
    }
}

void BookWidget::indexModelReady()
{
    m_indexWidget->setIndex(m_retModel.result());
}

void BookWidget::readerTextChanged()
{
    m_view->setHtml(m_db->text());
}
