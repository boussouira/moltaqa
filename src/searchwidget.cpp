#include "searchwidget.h"
#include "ui_searchwidget.h"
#include "clheader.h"
#include "clutils.h"
#include "clconstants.h"
#include "arabicanalyzer.h"
#include "searchfiltermanager.h"
#include <qmenu.h>
#include <qmessagebox.h>
#include <qdebug.h>
#include "mainwindow.h"
#include "librarysearcher.h"

SearchWidget::SearchWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchWidget),
    m_searcher(0)
{
    ui->setupUi(this);

    m_filterManager = new SearchFilterManager;
    m_filterManager->setLineEdit(ui->lineFilter);

    ui->treeViewBooks->setModel(m_filterManager->filterModel());
    ui->treeViewBooks->resizeColumnToContents(0);

    m_resultWidget = new ResultWidget(this);
    ui->stackedWidget->insertWidget(1, m_resultWidget);

    connect(ui->lineFilter, SIGNAL(textChanged(QString)),
            ui->treeViewBooks, SLOT(expandAll()));

    connect(ui->lineQueryMust, SIGNAL(returnPressed()), SLOT(search()));
    connect(ui->lineQueryShould, SIGNAL(returnPressed()), SLOT(search()));
    connect(ui->lineQueryShouldNot, SIGNAL(returnPressed()), SLOT(search()));
    connect(ui->pushSearch, SIGNAL(clicked()), SLOT(search()));

    setupCleanMenu();

    setCurrentWidget(Search);
    ui->lineQueryMust->setText(tr("الله اعلم"));
}

SearchWidget::~SearchWidget()
{
    if(!m_searcher) {
        delete m_searcher;
        m_searcher = 0;
    }

    delete m_resultWidget;
    delete m_filterManager;
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

Query *SearchWidget::getSearchQuery()
{
    if(ui->lineQueryMust->text().isEmpty()){
        if(!ui->lineQueryShould->text().isEmpty()){
            ui->lineQueryMust->setText(ui->lineQueryShould->text());
            ui->lineQueryShould->clear();
        } else {
            QMessageBox::warning(this,
                                 tr("البحث"),
                                 tr("يجب ملء حقل العبارات التي يجب ان تظهر في النتائج"));
            return 0;
        }
    }

    QString mustQureyStr = ui->lineQueryMust->text();
    QString shouldQureyStr = ui->lineQueryShould->text();
    QString shouldNotQureyStr = ui->lineQueryShouldNot->text();

    ArabicAnalyzer analyzer;
    BooleanQuery *q = new BooleanQuery;

    QueryParser queryPareser(PAGE_TEXT_FIELD, &analyzer);
    queryPareser.setAllowLeadingWildcard(true);

    try {
        if(!mustQureyStr.isEmpty()) {
            if(ui->checkQueryMust->isChecked())
                queryPareser.setDefaultOperator(QueryParser::AND_OPERATOR);
            else
                queryPareser.setDefaultOperator(QueryParser::OR_OPERATOR);

            wchar_t *queryText = Utils::QStringToWChar(mustQureyStr);
            Query *mq = queryPareser.parse(queryText);
            q->add(mq, BooleanClause::MUST);

            free(queryText);
        }

        if(!shouldQureyStr.isEmpty()) {
            if(ui->checkQueryShould->isChecked())
                queryPareser.setDefaultOperator(QueryParser::AND_OPERATOR);
            else
                queryPareser.setDefaultOperator(QueryParser::OR_OPERATOR);

            wchar_t *queryText = Utils::QStringToWChar(shouldQureyStr);
            Query *mq = queryPareser.parse(queryText);
            q->add(mq, BooleanClause::SHOULD);

            free(queryText);
        }

        if(!shouldNotQureyStr.isEmpty()) {
            if(ui->checkQueryShouldNot->isChecked())
                queryPareser.setDefaultOperator(QueryParser::AND_OPERATOR);
            else
                queryPareser.setDefaultOperator(QueryParser::OR_OPERATOR);

            wchar_t *queryText = Utils::QStringToWChar(shouldNotQureyStr);
            Query *mq = queryPareser.parse(queryText);
            q->add(mq, BooleanClause::MUST_NOT);

            free(queryText);
        }

//        qDebug() << "Search:" << Utils::WCharToString(q->toString(PAGE_TEXT_FIELD));

        return q;

    } catch(CLuceneError &e) {
        if(e.number() == CL_ERR_Parse)
            QMessageBox::warning(this,
                                 tr("خطأ في استعلام البحث"),
                                 tr("هنالك خطأ في احدى حقول البحث"
                                    "\n"
                                    "تأكد من حذف الأقواس و المعقوفات وغيرها،"
                                    " ويمكنك فعل ذلك من خلال زر التنظيف الموجود يسار حقل البحث، بعد الضغط على هذا الزر اعد البحث"
                                    "\n"
                                    "او تأكد من أنك تستخدمها بشكل صحيح"));
        else
            QMessageBox::warning(0,
                                 "CLucene Query error",
                                 tr("code: %1\nError: %2").arg(e.number()).arg(e.what()));

        _CLDELETE(q);

        return 0;
    }
    catch(...) {
        QMessageBox::warning(0,
                             "CLucene Query error",
                             tr("Unknow error"));
        _CLDELETE(q);

        return 0;
    }
}

void SearchWidget::testSearch()
{
    qDebug("Test Search..");
    try {
        SearchFilter *searchFilter = m_filterManager->getFilterQuery();
        Query *searchQuery = getSearchQuery();
        if(!searchQuery) {
            qDebug("No query...");
            return;
        }

        LibrarySearcher se;
        se.setQuery(searchQuery, searchFilter->filterQuery, searchFilter->clause);
        se.run();
        qDebug("Result count: %d", se.resultsCount());
    } catch(CLuceneError &e) {
        qDebug() << "Error:" << e.what();
    }
}

void SearchWidget::search()
{
    SearchFilter *searchFilter = m_filterManager->getFilterQuery();
    Query *searchQuery = getSearchQuery();

    if(!searchQuery)
        return;

    if(!m_searcher) {
        delete m_searcher;
        m_searcher = 0;
    }

    m_searcher = new LibrarySearcher(this);
    m_searcher->setQuery(searchQuery, searchFilter->filterQuery, searchFilter->clause);

    m_resultWidget->search(m_searcher);
    setCurrentWidget(Result);
}
