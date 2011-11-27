#include "searchwidget.h"
#include "ui_searchwidget.h"
#include "searchfiltermanager.h"
#include "clutils.h"
#include "resultwidget.h"

SearchWidget::SearchWidget(QWidget *parent) :
    QWidget(parent),
    m_resultWidget(0),
    m_searcher(0),
    ui(new Ui::SearchWidget)
{
    ui->setupUi(this);

    connect(ui->lineFilter, SIGNAL(textChanged(QString)),
            ui->treeView, SLOT(expandAll()));

    connect(ui->lineQueryMust, SIGNAL(returnPressed()), SLOT(search()));
    connect(ui->lineQueryShould, SIGNAL(returnPressed()), SLOT(search()));
    connect(ui->lineQueryShouldNot, SIGNAL(returnPressed()), SLOT(search()));
    connect(ui->pushSearch, SIGNAL(clicked()), SLOT(search()));
    connect(ui->labelTools, SIGNAL(linkActivated(QString)),
            SLOT(showFilterTools()));

    setupCleanMenu();
    setCurrentWidget(Search);
}

SearchWidget::~SearchWidget()
{
    if(m_searcher)
        delete m_searcher;

    if(m_resultWidget)
        delete m_resultWidget;

    delete ui;
}

void SearchWidget::setCurrentWidget(SearchWidget::CurrentWidget index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

void SearchWidget::toggleWidget()
{
    if(m_searcher) // Do we have any search result?
        setCurrentWidget(ui->stackedWidget->currentIndex()==Search ? Result : Search);
}

void SearchWidget::search()
{
    SearchFilter *searchFilter = getSearchFilterQuery();
    Query *searchQuery = getSearchQuery();

    if(!searchQuery)
        return;

    if(m_searcher) {
        delete m_searcher;
        m_searcher = 0;
    }

    m_searcher = new LibrarySearcher(this);
    m_searcher->setQuery(searchQuery, searchFilter->filterQuery, searchFilter->clause);

    m_resultWidget->search(m_searcher);
    setCurrentWidget(Result);
}

void SearchWidget::setupCleanMenu()
{
    QList<FancyLineEdit*> lines;
    lines << ui->lineQueryMust;
    lines << ui->lineQueryShould;
    lines << ui->lineQueryShouldNot;

    foreach(FancyLineEdit *line, lines) {
        QMenu *menu = new QMenu(line);
        QAction *clearTextAct = new QAction(tr("مسح النص"), line);
        QAction *clearSpecialCharAct = new QAction(tr("ابطال مفعول الاقواس وغيرها"), line);

        menu->addAction(clearTextAct);
        menu->addAction(clearSpecialCharAct);

        connect(clearTextAct, SIGNAL(triggered()), SLOT(clearLineText()));
        connect(clearSpecialCharAct, SIGNAL(triggered()), SLOT(clearSpecialChar()));

        line->setMenu(menu);
    }
}

void SearchWidget::clearLineText()
{
    FancyLineEdit *edit = qobject_cast<FancyLineEdit*>(sender()->parent());

    if(edit) {
        edit->clear();
    }
}

void SearchWidget::showFilterTools()
{
    QMenu menu(this);

    menu.addAction(tr("اختيار الكل"), this, SLOT(selectAll()));
    menu.addAction(tr("الغاء الكل"), this, SLOT(unSelectAll()));
    menu.addSeparator();
    menu.addAction(tr("اختيار الكتب الظاهرة فقط"), this, SLOT(selectVisible()));
    menu.addAction(tr("الغاء الكتب الظاهرة فقط"), this, SLOT(unSelectVisible()));
    menu.addSeparator();
    menu.addAction(tr("عرض الشجرة"), this, SLOT(expandFilterView()));
    menu.addAction(tr("ضغط الشجرة"), this, SLOT(collapseFilterView()));

    menu.exec(QCursor::pos());
}

void SearchWidget::clearSpecialChar()
{
    FancyLineEdit *edit = qobject_cast<FancyLineEdit*>(sender()->parent());

    if(edit) {
        wchar_t *lineText = Utils::QStringToWChar(edit->text());
        wchar_t *cleanText = QueryParser::escape(lineText);

        edit->setText(QString::fromWCharArray(cleanText));

        free(lineText);
        free(cleanText);
    }
}
