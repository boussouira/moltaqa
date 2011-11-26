#include "librarysearchwidget.h"
#include "ui_librarysearchwidget.h"
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

LibrarySearchWidget::LibrarySearchWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LibrarySearchWidget),
    m_searcher(0)
{
    ui->setupUi(this);

    m_filterManager = new SearchFilterManager;
    m_filterManager->setLineEdit(ui->lineFilter);

    ui->treeViewBooks->setModel(m_filterManager->filterModel());
    ui->treeViewBooks->setColumnWidth(0, 300);

    m_resultWidget = new ResultWidget(this);
    ui->stackedWidget->insertWidget(1, m_resultWidget);

    connect(ui->lineFilter, SIGNAL(textChanged(QString)),
            ui->treeViewBooks, SLOT(expandAll()));

    connect(ui->lineQueryMust, SIGNAL(returnPressed()), SLOT(search()));
    connect(ui->lineQueryShould, SIGNAL(returnPressed()), SLOT(search()));
    connect(ui->lineQueryShouldNot, SIGNAL(returnPressed()), SLOT(search()));
    connect(ui->pushSearch, SIGNAL(clicked()), SLOT(search()));
    connect(ui->labelTools, SIGNAL(linkActivated(QString)),
            SLOT(showFilterTools()));

    setupCleanMenu();

    setCurrentWidget(Search);
    ui->lineQueryMust->setText(tr("الله اعلم"));
}

LibrarySearchWidget::~LibrarySearchWidget()
{
    if(!m_searcher) {
        delete m_searcher;
        m_searcher = 0;
    }

    delete m_resultWidget;
    delete m_filterManager;
    delete ui;
}

void LibrarySearchWidget::setCurrentWidget(LibrarySearchWidget::CurrentWidget index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

void LibrarySearchWidget::toggleWidget()
{
    if(m_searcher) // Do we have any search result?
        setCurrentWidget(ui->stackedWidget->currentIndex()==Search ? Result : Search);
}

void LibrarySearchWidget::setupCleanMenu()
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

void LibrarySearchWidget::clearLineText()
{
    FancyLineEdit *edit = qobject_cast<FancyLineEdit*>(sender()->parent());

    if(edit) {
        edit->clear();
    }
}

void LibrarySearchWidget::showFilterTools()
{
    QMenu menu(this);
    QAction *selectAllAct = new QAction(tr("اختيار الكل"), &menu);
    QAction *unSelectAllAct = new QAction(tr("الغاء الكل"), &menu);

    QAction *selectVisisbleAct = new QAction(tr("اختيار الكتب الظاهرة فقط"), &menu);
    QAction *unSelectVisisbleAct = new QAction(tr("الغاء الكتب الظاهرة فقط"), &menu);

    QAction *expandTreeAct = new QAction(tr("عرض الشجرة"), &menu);
    QAction *collapseTreeAct = new QAction(tr("ضغط الشجرة"), &menu);

    menu.addAction(selectAllAct);
    menu.addAction(unSelectAllAct);
    menu.addSeparator();
    menu.addAction(selectVisisbleAct);
    menu.addAction(unSelectVisisbleAct);
    menu.addSeparator();
    menu.addAction(expandTreeAct);
    menu.addAction(collapseTreeAct);

    QAction *ret = menu.exec(QCursor::pos());
    if(ret) {
        if(ret == selectAllAct) {
            m_filterManager->selectAllBooks();
        } else if(ret == unSelectAllAct) {
            m_filterManager->unSelectAllBooks();
        } else if(ret == selectVisisbleAct) {
            m_filterManager->selectVisibleBooks();
        } else if(ret == unSelectVisisbleAct) {
            m_filterManager->unSelectVisibleBooks();
        } else if(ret == expandTreeAct) {
            ui->treeViewBooks->expandAll();
        } else if(ret == collapseTreeAct) {
            ui->treeViewBooks->collapseAll();
        }
    }
}

void LibrarySearchWidget::clearSpecialChar()
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

Query *LibrarySearchWidget::getSearchQuery()
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

void LibrarySearchWidget::search()
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
