#include "bookwidget.h"
#include "indexwidget.h"
#include "abstractdbhandler.h"
#include "kwebview.h"

#include <qsplitter.h>
#include <qboxlayout.h>

BookWidget::BookWidget(AbstractDBHandler *db, QWidget *parent): QWidget(parent), m_db(db)
{
    m_layout = new QVBoxLayout(this);
    m_splitter = new QSplitter(this);
    m_view = new KWebView(m_splitter);
    m_indexWidget = new IndexWidget(m_splitter);
    m_indexWidget->setBookInfo(db->bookInfo());

    m_splitter->addWidget(m_indexWidget);
    m_splitter->addWidget(m_view);
    m_layout->addWidget(m_splitter);
    m_layout->setMargin(0);
    setLayout(m_layout);
    setAutoFillBackground(true);
    m_splitter->setChildrenCollapsible(false);

    displayInfo();
    connect(m_indexWidget, SIGNAL(openPage(int)), this, SLOT(openID(int)));
}

BookWidget::~BookWidget()
{
    delete m_db;
}

void BookWidget::setDBHandler(AbstractDBHandler *db)
{
    delete m_db;
    m_db = db;
    m_indexWidget->setBookInfo(db->bookInfo());
}

void BookWidget::displayInfo()
{
    m_indexWidget->setIndex(m_db->indexModel());
    m_indexWidget->hideAyaSpin(m_db->bookInfo()->isQuran());
    m_indexWidget->hidePartSpin(m_db->bookInfo()->partsCount() > 1);
}

void BookWidget::openID(int id)
{
    m_view->setHtml(m_db->openIndexID(id));
    if(m_db->bookInfo()->isQuran())
        m_view->scrollToSora(id);
    m_indexWidget->displayBookInfo();
}

void BookWidget::firstPage()
{
    m_view->setHtml(m_db->openIndexID());
    if(m_db->bookInfo()->isQuran())
        m_view->scrollToSora(1); // First sora
    m_indexWidget->displayBookInfo();
}

void BookWidget::nextPage()
{
    if(dbHandler()->hasNext()) {
        m_view->setHtml(m_db->nextPage());
        if(m_db->bookInfo()->isQuran())
            m_view->scrollToAya(m_db->bookInfo()->currentSoraNumber(),
                                m_db->bookInfo()->currentAya());
        m_indexWidget->displayBookInfo();
    }
}

void BookWidget::prevPage()
{
    if(dbHandler()->hasPrev()) {
        m_view->setHtml(m_db->prevPage());
        if(m_db->bookInfo()->isQuran())
            m_view->scrollToAya(m_db->bookInfo()->currentSoraNumber(),
                                m_db->bookInfo()->currentAya());
        m_indexWidget->displayBookInfo();
    }
}

void BookWidget::nextUnit()
{
    if(m_db->bookInfo()->isQuran()) {
        QString page = m_db->nextUnit();
        if(!page.isEmpty())
            m_view->setHtml(page);
        m_view->scrollToAya(m_db->bookInfo()->currentSoraNumber(),
                            m_db->bookInfo()->currentAya());
        m_indexWidget->displayBookInfo();
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
        QString page = m_db->prevUnit();
        if(!page.isEmpty())
            m_view->setHtml(page);
        m_view->scrollToAya(m_db->bookInfo()->currentSoraNumber(),
                            m_db->bookInfo()->currentAya());
        m_indexWidget->displayBookInfo();
    } else {
        if(!m_view->maxUp())
            m_view->pageUp();
        else
            prevPage();
    }
}

void BookWidget::hideIndexWidget()
{
    if(m_indexWidget->isHidden())
        m_indexWidget->show();
    else
        m_indexWidget->hide();
}
