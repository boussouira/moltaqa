#include "bookwidget.h"

BookWidget::BookWidget(AbstractDBHandler *db, QWidget *parent): QWidget(parent), m_db(db)
{
    m_layout = new QVBoxLayout(this);
    m_splitter = new QSplitter(this);
    m_view = new KWebView(m_splitter);
    m_indexWidget = new IndexWidget(m_splitter);

    m_splitter->addWidget(m_indexWidget);
    m_splitter->addWidget(m_view);
    m_layout->addWidget(m_splitter);
    m_layout->setMargin(0);
    setLayout(m_layout);

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
}

void BookWidget::displayInfo()
{
    m_indexWidget->setIndex(m_db->indexModel());
    m_indexWidget->hideAyaSpin(m_db->bookInfo()->bookType()==BookInfo::QuranBook);
    m_indexWidget->hidePartSpin(m_db->bookInfo()->partsCount() > 1);
}

void BookWidget::openID(int id)
{
    m_view->setHtml(m_db->page(id));
}

void BookWidget::firstPage()
{
    m_view->setHtml(m_db->page());
}

void BookWidget::nextPage()
{
    m_view->setHtml(m_db->nextPage());
}

void BookWidget::prevPage()
{
    m_view->setHtml(m_db->prevPage());
}

void BookWidget::nextUnit()
{
    if(m_db->bookInfo()->bookType() == BookInfo::NormalBook) {
        if(!m_view->maxDown())
            m_view->pageDown();
        else
            nextPage();
    }
}

void BookWidget::prevUnit()
{
    if(m_db->bookInfo()->bookType() == BookInfo::NormalBook) {
        if(!m_view->maxUp())
            m_view->pageUp();
        else
            prevPage();
    }
}
