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
#include "clucenequery.h"
#include "utils.h"
#include "searchmanager.h"
#include "searchfieldsdialog.h"
#include <qmessagebox.h>
#include <qinputdialog.h>

SearchWidget::SearchWidget(QWidget *parent) :
    QWidget(parent),
    m_resultWidget(0),
    m_searcher(0),
    m_filterManager(0),
    ui(new Ui::SearchWidget)
{
    ui->setupUi(this);

    connect(ui->lineQueryMust, SIGNAL(returnPressed()), SLOT(search()));
    connect(ui->lineQueryShould, SIGNAL(returnPressed()), SLOT(search()));
    connect(ui->lineQueryShouldNot, SIGNAL(returnPressed()), SLOT(search()));
    connect(ui->pushSearch, SIGNAL(clicked()), SLOT(search()));
    connect(ui->labelTools, SIGNAL(linkActivated(QString)),
            SLOT(showFilterTools()));
    connect(ui->labelSearchField, SIGNAL(linkActivated(QString)),
            SLOT(showSearchFieldMenu()));

    setupCleanMenu();
    setCurrentWidget(Search);
}

SearchWidget::~SearchWidget()
{
    if(m_searcher) {
        if(m_searcher->isRunning()) {
            m_searcher->stop();
            m_searcher->wait();
        }

        delete m_searcher;
    }

    ml_delete_check(m_resultWidget);
    ml_delete_check(m_filterManager);

    delete ui;
}

void SearchWidget::setCurrentWidget(SearchWidget::CurrentWidget index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

SearchWidget::CurrentWidget SearchWidget::currentWidget()
{
    return static_cast<CurrentWidget>(ui->stackedWidget->currentIndex());
}

void SearchWidget::toggleWidget()
{
    if(m_searcher) // Do we have any search result?
        setCurrentWidget(ui->stackedWidget->currentIndex()==Search ? Result : Search);
}

void SearchWidget::showSearchInfo()
{
    ml_return_on_fail(m_searcher);
    ml_return_on_fail(m_searcher->getSearchQuery());

    QString sec = QString::number(m_searcher->searchTime()/1000.);
    if(sec.indexOf('.') != -1)
        sec = sec.left(sec.indexOf('.')+5);

    QString info;
    info += tr("تم البحث خلال: %1 ثانية").arg(sec);

    CLuceneQuery *query = m_searcher->getSearchQuery();
    if((query->resultFilter && query->resultFilter->clause == BooleanClause::MUST)
            || (query->filter && query->filter->clause == BooleanClause::MUST && query->filter->selected == 1)) {
        info += tr(" في كتاب واحد");
    } else {
        if(query->filter) {
            int selected = query->filter->selected;
            info += tr(" في %1 كتاب").arg(selected);

        } else {
            info += tr(" في كل المكتبة");
        }

        if(query->resultFilter) {
            info += tr(" مع استبعاد %1 كتاب").arg(query->resultFilter->unSelected);
        }
    }

    info += "\n";
    info += tr("عدد نتائج البحث: %1").arg(m_searcher->resultsCount());

    QMessageBox::information(this,
                             tr("نتائج البحث"),
                             info);
}

void SearchWidget::setSearchText(QString mustQuery, QString shouldQuery, QString mustNotQuery)
{
    ui->lineQueryMust->setText(Utils::CLucene::clearSpecialChars(mustQuery));
    ui->lineQueryShould->setText(Utils::CLucene::clearSpecialChars(shouldQuery));
    ui->lineQueryShouldNot->setText(Utils::CLucene::clearSpecialChars(mustNotQuery));
}

Query *SearchWidget::getSearchQuery(const wchar_t *searchField)
{
    if(ui->lineQueryMust->text().isEmpty()){
        if(ui->lineQueryShould->text().size()){
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

    QueryParser queryPareser(searchField, &analyzer);
    queryPareser.setAllowLeadingWildcard(true);

    try {
        if(mustQureyStr.size()) {
            if(ui->checkQueryMust->isChecked())
                queryPareser.setDefaultOperator(QueryParser::AND_OPERATOR);
            else
                queryPareser.setDefaultOperator(QueryParser::OR_OPERATOR);

            wchar_t *queryText = Utils::CLucene::QStringToWChar(mustQureyStr);
            Query *mq = queryPareser.parse(queryText);
            q->add(mq, true, BooleanClause::MUST);

            free(queryText);
        }

        if(shouldQureyStr.size()) {
            if(ui->checkQueryShould->isChecked())
                queryPareser.setDefaultOperator(QueryParser::AND_OPERATOR);
            else
                queryPareser.setDefaultOperator(QueryParser::OR_OPERATOR);

            wchar_t *queryText = Utils::CLucene::QStringToWChar(shouldQureyStr);
            Query *mq = queryPareser.parse(queryText);
            q->add(mq, true, BooleanClause::SHOULD);

            free(queryText);
        }

        if(shouldNotQureyStr.size()) {
            if(ui->checkQueryShouldNot->isChecked())
                queryPareser.setDefaultOperator(QueryParser::AND_OPERATOR);
            else
                queryPareser.setDefaultOperator(QueryParser::OR_OPERATOR);

            wchar_t *queryText = Utils::CLucene::QStringToWChar(shouldNotQureyStr);
            Query *mq = queryPareser.parse(queryText);
            q->add(mq, BooleanClause::MUST_NOT);

            free(queryText);
        }

        QString queryStr;
        if(mustQureyStr.size())
            queryStr += "+(" + mustQureyStr + ") ";

        if(shouldQureyStr.size())
            queryStr += "(" + shouldQureyStr + ") ";

        if(shouldNotQureyStr.size())
            queryStr += "-(" + shouldNotQureyStr + ") ";

        qDebug() << "Search: Query" << queryStr.trimmed();

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

        ml_delete(q);

        return 0;
    }
    catch(...) {
        QMessageBox::warning(0,
                             "CLucene Query error",
                             tr("Unknow error"));
        ml_delete(q);

        return 0;
    }
}

QString SearchWidget::getSearchField()
{
    const wchar_t *field = 0;
    switch(ui->comboSearchField->currentIndex()) {
    case 0:
        field = PAGE_TEXT_FIELD;
        break;
    case 1:
        field = TITLE_TEXT_FIELD;
        break;
    case 2:
        field = HADDIT_MATEEN_FIELD;
        break;
    case 3:
        field = HADDIT_SANAD_FIELD;
        break;
    case 4:
        field = SHEER_FIELD;
        break;
    default:
        field = PAGE_TEXT_FIELD;
    }

    return Utils::CLucene::WCharToString(field);
}

void SearchWidget::search()
{
    QString searchField = getSearchField();
    wchar_t *searchFieldW = Utils::CLucene::QStringToWChar(searchField);

    SearchFilter *searchFilter = getSearchFilterQuery();
    Query *searchQuery = getSearchQuery(searchFieldW);

    ml_return_on_fail(searchQuery);

    CLuceneQuery *query = new CLuceneQuery();
    query->searchQuery = searchQuery;
    query->filter = searchFilter;
    query->searchField = searchField;
    query->searchFieldW = searchFieldW;
    query->sort = static_cast<CLuceneQuery::SearchSort>(ui->comboSortSearch->currentIndex());

    if(m_searcher) {
        if(m_searcher->isRunning()) {
            m_searcher->stop();
            m_searcher->wait();
        }

        ml_delete(m_searcher);
    }

    m_searcher = new LibrarySearcher(this);
    m_searcher->setQuery(query);

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
    ml_return_on_fail(m_filterManager);

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

void SearchWidget::showSearchFieldMenu()
{
    QMenu menu(this);

    menu.addAction(tr("حفظ الكتب المحددة"), this, SLOT(saveSelectedField()));
    menu.addAction(tr("تعديل مجالات البحث..."), this, SLOT(searchfieldsDialog()));
    menu.addSeparator();
    QMenu *sub = menu.addMenu(tr("تغيير مجال البحث"));

    QList<SearchFieldInfo> fields = LibraryManager::instance()->searchManager()->getFieldNames();
    foreach(SearchFieldInfo info, fields) {
        QAction *act = sub->addAction(info.name);
        act->setData(info.fieldID);

        connect(act, SIGNAL(triggered()), this, SLOT(changeSearchfield()));
    }

    if(fields.isEmpty()) {
        QAction *act = sub->addAction(tr("(لا يوجد اي مجال بحث)"));
        act->setEnabled(false);
    }

    menu.exec(QCursor::pos());
}

void SearchWidget::saveSelectedField()
{
    QList<int> selectBooks = m_filterManager->getSelectedItems();
    if(!selectBooks.isEmpty()) {
        QString name = QInputDialog::getText(this,
                                             tr("حفظ مجال البحث"),
                                             tr("اسم مجال البحث:")).trimmed();
        if(name.isEmpty()) {
            QMessageBox::warning(this,
                                 tr("حفظ مجال البحث"),
                                 tr("يجب ان تختار اسما لمجال البحث"));
        } else {
            if(LibraryManager::instance()->searchManager()->addField(name, selectBooks)) {
                QMessageBox::information(this,
                                     tr("حفظ مجال البحث"),
                                     tr("تم حفظ مجال البحث"));
            }
        }
    } else {
        QMessageBox::warning(this,
                             tr("حفظ مجال البحث"),
                             tr("لم تقم باختيار اي كتاب!"));
    }
}

void SearchWidget::searchfieldsDialog()
{
    SearchFieldsDialog dialog(this);
    dialog.exec();
}

void SearchWidget::changeSearchfield()
{
    QAction *act = qobject_cast<QAction*>(sender());
    ml_return_on_fail2(act, "SearchWidget::changeSearchfield sender action is null");

    QList<int> selectBooks = LibraryManager::instance()->searchManager()->getFieldBooks(act->data().toInt());
    m_filterManager->setSelectedItems(selectBooks);
}

void SearchWidget::clearSpecialChar()
{
    FancyLineEdit *edit = qobject_cast<FancyLineEdit*>(sender()->parent());

    if(edit) {
        edit->setText(Utils::CLucene::clearSpecialChars(edit->text()));
    }
}
