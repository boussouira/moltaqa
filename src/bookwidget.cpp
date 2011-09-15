#include "bookwidget.h"
#include "indexwidget.h"
#include "abstractdbhandler.h"
#include "webview.h"
#include "textformatter.h"
#include "simpledbhandler.h"

#include <qsplitter.h>
#include <qboxlayout.h>
#include <qtconcurrentrun.h>
#include <qfuturewatcher.h>
#include <QCloseEvent>
#include <QFile>

BookWidget::BookWidget(AbstractDBHandler *db, QWidget *parent): QWidget(parent), m_db(db)
{
    m_layout = new QVBoxLayout(this);
    m_splitter = new QSplitter(this);
    m_view = new WebView(m_splitter);
    m_indexWidget = new IndexWidget(m_splitter);
    m_indexWidget->setBookInfo(db->bookInfo());

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
    connect(m_indexWidget, SIGNAL(openPage(int)), this, SLOT(openID(int)));
    connect(m_db->textFormatter(), SIGNAL(doneReading(QString)), m_view, SLOT(setText(QString))); // TODO: don't call setText directly
    connect(m_view, SIGNAL(textChanged()), m_indexWidget, SLOT(displayBookInfo()));
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

void BookWidget::setDBHandler(AbstractDBHandler *db)
{
    m_db = db;
    m_indexWidget->setBookInfo(db->bookInfo());
    connect(db->textFormatter(), SIGNAL(doneReading(QString)), m_view, SLOT(setText(QString))); // TODO: don't call setText directly
}

void BookWidget::displayInfo()
{
    if(m_db->needFastIndexLoad()) {
        m_retModel = QtConcurrent::run(m_db, &AbstractDBHandler::indexModel);
        m_watcher->setFuture(m_retModel);

        m_indexWidget->setIndex(m_db->topIndexModel());
    } else {
        m_indexWidget->setIndex(m_db->indexModel());
    }

    m_indexWidget->hideAyaSpin(m_db->bookInfo()->isQuran() || m_db->bookInfo()->isTafessir());
    m_indexWidget->hidePartSpin(m_db->bookInfo()->partsCount > 1);
}

void BookWidget::openID(int id)
{
    m_db->openIndexID(id);

    if(m_db->bookInfo()->isQuran())
        m_view->scrollToSora(id);
}

void BookWidget::firstPage()
{
    m_db->openIndexID();

    if(m_db->bookInfo()->isQuran())
        m_view->scrollToSora(1); // First sora
}

void BookWidget::lastPage()
{
    if(m_db->bookInfo()->isQuran()) {
        m_db->goToPage(m_db->bookInfo()->lastPage(), 1);
        m_view->scrollToAya(112, 1);
    } else {
        m_db->openIndexID(-2);
    }
}

void BookWidget::nextPage()
{
    if(dbHandler()->hasNext()) {
       m_db->nextPage();
        if(m_db->bookInfo()->isQuran())
            m_view->scrollToAya(m_db->bookInfo()->currentSoraNumber,
                                m_db->bookInfo()->currentAyaNumber);
    }
}

void BookWidget::prevPage()
{
    if(dbHandler()->hasPrev()) {
        m_db->prevPage();
        if(m_db->bookInfo()->isQuran())
            m_view->scrollToAya(m_db->bookInfo()->currentSoraNumber,
                                m_db->bookInfo()->currentAyaNumber);
    }
}

void BookWidget::nextUnit()
{
    if(m_db->bookInfo()->isQuran()) {
        m_db->nextUnit();
        m_view->scrollToAya(m_db->bookInfo()->currentSoraNumber,
                            m_db->bookInfo()->currentAyaNumber);
    } else {
        if(!m_view->maxDown())
            m_view->pageDown();
        else
            nextPage();
    }
}

void BookWidget::prevUnit()
{
    if(m_db->bookInfo()->isQuran()) {
        m_db->prevUnit();
        m_view->scrollToAya(m_db->bookInfo()->currentSoraNumber,
                            m_db->bookInfo()->currentAyaNumber);
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
        m_view->scrollToAya(m_db->bookInfo()->currentSoraNumber,
                            m_db->bookInfo()->currentAyaNumber);
}

void BookWidget::openSora(int sora, int aya)
{
    if(m_db->bookInfo()->isQuran() || m_db->bookInfo()->isTafessir())
   {     m_db->goToSora(sora, aya);

    if(m_db->bookInfo()->isQuran())
        m_view->scrollToAya(m_db->bookInfo()->currentSoraNumber,
                            m_db->bookInfo()->currentAyaNumber);
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
