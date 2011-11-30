#include "searchwidget.h"
#include "ui_searchwidget.h"
#include "searchfiltermanager.h"
#include "resultwidget.h"
#include "clheader.h"
#include "clutils.h"
#include "clconstants.h"
#include "arabicanalyzer.h"
#include "librarysearcher.h"
#include "booksearchfilter.h"
#include <qmessagebox.h>

SearchWidget::SearchWidget(QWidget *parent) :
    QWidget(parent),
    m_resultWidget(0),
    m_searcher(0),
    m_filterManager(0),
    ui(new Ui::SearchWidget)
{
    ui->setupUi(this);

    ui->lineQueryMust->setText(tr("الله"));

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

    if(m_filterManager)
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
    if(!m_filterManager)
        return;

    QMenu menu(this);

    menu.addAction(tr("اختيار الكل"), m_filterManager, SLOT(selectAll()));
    menu.addAction(tr("الغاء الكل"), m_filterManager, SLOT(unSelectAll()));
    menu.addSeparator();
    menu.addAction(tr("اختيار الكتب الظاهرة فقط"), m_filterManager, SLOT(selectVisible()));
    menu.addAction(tr("الغاء الكتب الظاهرة فقط"), m_filterManager, SLOT(unSelectVisible()));
    menu.addSeparator();
    menu.addAction(tr("عرض الشجرة"), m_filterManager, SLOT(expandFilterView()));
    menu.addAction(tr("ضغط الشجرة"), m_filterManager, SLOT(collapseFilterView()));

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
