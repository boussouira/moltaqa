#include "librarysearchwidget.h"
#include "ui_searchwidget.h"
#include "resultwidget.h"
#include "clheader.h"
#include "clutils.h"
#include "clconstants.h"
#include "arabicanalyzer.h"
#include "searchfiltermanager.h"
#include "librarysearcher.h"
#include <qmessagebox.h>

LibrarySearchWidget::LibrarySearchWidget(QWidget *parent) :
    SearchWidget(parent),
    m_filterManager(0)
{
    ui->lineQueryMust->setText(tr("الله اعلم"));
}

LibrarySearchWidget::~LibrarySearchWidget()
{
    if(m_filterManager)
        delete m_filterManager;
}

void LibrarySearchWidget::init()
{
    m_filterManager = new SearchFilterManager;
    m_filterManager->setTreeView(ui->treeView);
    m_filterManager->setLineEdit(ui->lineFilter);

    m_resultWidget = new ResultWidget(this);
    ui->stackedWidget->insertWidget(1, m_resultWidget);

    ui->treeView->setModel(m_filterManager->filterModel());
    ui->treeView->setColumnWidth(0, 300);
}

void LibrarySearchWidget::selectAll()
{
    m_filterManager->selectAllBooks();
}

void LibrarySearchWidget::unSelectAll()
{
    m_filterManager->unSelectAllBooks();
}

void LibrarySearchWidget::selectVisible()
{
    m_filterManager->selectVisibleBooks();
}

void LibrarySearchWidget::unSelectVisible()
{
    m_filterManager->unSelectVisibleBooks();
}

void LibrarySearchWidget::expandFilterView()
{
    m_filterManager->expandFilterView();
}

void LibrarySearchWidget::collapseFilterView()
{
    m_filterManager->collapseFilterView();
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

SearchFilter *LibrarySearchWidget::getSearchFilterQuery()
{
    return m_filterManager->getFilterQuery();
}
