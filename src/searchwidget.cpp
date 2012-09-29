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
#include "statisticsmanager.h"
#include "searchresultreader.h"

#include <qmessagebox.h>
#include <qinputdialog.h>
#include <qcompleter.h>
#include <qsettings.h>

#define ADD_QUERY(text, andOperator, clause) { \
    Query *sq = Utils::CLucene::parse(&queryPareser, text, ui->andOperator->isChecked()); \
    if(sq) q->add(sq, true, BooleanClause::clause);}

SearchWidget::SearchWidget(QWidget *parent) :
    QWidget(parent),
    m_resultWidget(0),
    m_searcher(0),
    m_filterManager(0),
    m_completerModel(0),
    m_completer(0),
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
    loadSettings();
    loadSearchQuery();
}

SearchWidget::~SearchWidget()
{
    saveSettings();

    if(m_searcher) {
        if(m_searcher->isRunning()) {
            m_searcher->stop();
            m_searcher->wait();
        }

        delete m_searcher;
    }

    ml_delete_check(m_resultWidget);
    ml_delete_check(m_filterManager);

    ml_delete_check(m_completerModel);
    ml_delete_check(m_completer);

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

ResultWidget *SearchWidget::resultWidget()
{
    return m_resultWidget;
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
    if(ui->lineQueryMust->text().trimmed().isEmpty()) {
        if(ui->lineQueryShould->text().trimmed().size()) {
            ui->lineQueryMust->setText(ui->lineQueryShould->text());
            ui->lineQueryShould->clear();
        }
    }

    QString mustQureyStr = ui->lineQueryMust->text().trimmed();
    QString shouldQureyStr = ui->lineQueryShould->text().trimmed();
    QString shouldNotQureyStr = ui->lineQueryShouldNot->text().trimmed();

    if(mustQureyStr.isEmpty()){
            QMessageBox::warning(this,
                                 tr("البحث"),
                                 tr("يجب ملء حقل العبارات التي يجب ان تظهر في النتائج"));
            return 0;
    }

    ArabicAnalyzer analyzer;
    BooleanQuery *q = new BooleanQuery;

    QueryParser queryPareser(searchField, &analyzer);
    queryPareser.setAllowLeadingWildcard(true);

    try {
        ADD_QUERY(mustQureyStr, checkQueryMust, MUST);
        ADD_QUERY(shouldQureyStr, checkQueryShould, SHOULD);
        ADD_QUERY(shouldNotQureyStr, checkQueryShouldNot, MUST_NOT);

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

void SearchWidget::loadDefaultSearchField()
{
    int currentField = Utils::Settings::get("Search/defaultField", -1).toInt();
    if(currentField == -2) {
        m_filterManager->selectAll();
    } else if(currentField != -1) {
        QList<int> selectBooks = LibraryManager::instance()->searchManager()->getFieldBooks(currentField);
        ml_return_on_fail2(selectBooks.size(),
                           "SearchWidget::loadDefaultSearchField field" << currentField << "doesn't have any book");

        m_filterManager->setSelectedItems(selectBooks);
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

void SearchWidget::loadSettings()
{
    QSettings settings;

    settings.beginGroup("SearchWidget");

    if(settings.value("saveSearchOptions", true).toBool()) {
        ui->comboSortSearch->setCurrentIndex(settings.value("sortSearch", 0).toInt());
        ui->comboSearchField->setCurrentIndex(settings.value("searchField", 0).toInt());

        ui->checkQueryMust->setChecked(settings.value("checkQueryMust", false).toBool());
        ui->checkQueryShould->setChecked(settings.value("checkQueryShould", false).toBool());
        ui->checkQueryShouldNot->setChecked(settings.value("checkQueryShouldNot", false).toBool());

        ui->checkShowPageInfo->setChecked(settings.value("showPageInfo", true).toBool());
        ui->checkShowResultTitles->setChecked(settings.value("showResultTitles", true).toBool());
    }
}

void SearchWidget::saveSettings()
{
    QSettings settings;

    settings.beginGroup("SearchWidget");

    settings.setValue("sortSearch", ui->comboSortSearch->currentIndex());
    settings.setValue("searchField", ui->comboSearchField->currentIndex());

    settings.setValue("checkQueryMust", ui->checkQueryMust->isChecked());
    settings.setValue("checkQueryShould", ui->checkQueryShould->isChecked());
    settings.setValue("checkQueryShouldNot", ui->checkQueryShouldNot->isChecked());

    settings.setValue("showPageInfo", ui->checkShowPageInfo->isChecked());
    settings.setValue("showResultTitles", ui->checkShowResultTitles->isChecked());
}

void SearchWidget::saveSearchQuery()
{
    ml_return_on_fail(Utils::Settings::get("Search/saveSearch", true).toBool());

    QStringList list;
    list << ui->lineQueryMust->text().trimmed()
         << ui->lineQueryShould->text().trimmed()
         << ui->lineQueryShouldNot->text().trimmed();

    LibraryManager::instance()->searchManager()->saveSearchQueries(list);

    QString queryStr;
    for(int i=0; i<list.size(); i++) {
        if(list[i].size()) {
            queryStr += ((i==0) ? "+(" : ((i==2) ? "-(" : "("));
            queryStr += list[i];
            queryStr += ") ";
        }
    }

    QString log = QString("query: [%1] take %2 ms, results count %3")
            .arg(queryStr.trimmed())
            .arg(m_searcher->searchTime())
            .arg(m_searcher->resultsCount());

    StatisticsManager::instance()->add("search", log);

    if(m_completerModel) {
        foreach (QString q, list) {
            if(q.size())
                m_completerModel->appendRow(new QStandardItem(q));
        }
    }
}

void SearchWidget::loadSearchQuery()
{
    ml_return_on_fail(Utils::Settings::get("Search/saveSearch", true).toBool());

    m_completerModel = LibraryManager::instance()->searchManager()->getSavedSearchModel();

    if(!m_completer) {
        m_completer = new QCompleter(m_completerModel, this);
        ui->lineQueryMust->setCompleter(m_completer);
        ui->lineQueryShould->setCompleter(m_completer);
        ui->lineQueryShouldNot->setCompleter(m_completer);
    }
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

    m_searcher->getResultReader()->setShowPageInfo(ui->checkShowPageInfo->isChecked());
    m_searcher->getResultReader()->setShowPageTitle(ui->checkShowResultTitles->isChecked());

    connect(m_searcher, SIGNAL(doneSearching()), SLOT(doneSearching()));

    m_resultWidget->search(m_searcher);
    setCurrentWidget(Result);
}

void SearchWidget::setupCleanMenu()
{
    QList<FilterLineEdit*> lines;
    lines << ui->lineQueryMust;
    lines << ui->lineQueryShould;
    lines << ui->lineQueryShouldNot;

    foreach(FilterLineEdit *line, lines) {
        QMenu *menu = new QMenu(line);
        QAction *clearSpecialCharAct = new QAction(tr("ابطال مفعول الاقواس وغيرها"), line);

        menu->addAction(clearSpecialCharAct);

        connect(clearSpecialCharAct, SIGNAL(triggered()), SLOT(clearSpecialChar()));

        line->setFilterMenu(menu);
    }
}

void SearchWidget::showFilterTools()
{
    ml_return_on_fail(m_filterManager);

    QMenu menu(this);

    menu.addAction(tr("اختيار الكل"), m_filterManager, SLOT(selectAll()));
    menu.addAction(tr("إلغاء الكل"), m_filterManager, SLOT(unSelectAll()));
    menu.addSeparator();
    menu.addAction(tr("اختيار الكتب الظاهرة فقط"), m_filterManager, SLOT(selectVisible()));
    menu.addAction(tr("إلغاء الكتب الظاهرة فقط"), m_filterManager, SLOT(unSelectVisible()));
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

void SearchWidget::doneSearching()
{
    if(Utils::Settings::get("Search/showMessageAfterSearch", false).toBool())
        showSearchInfo();

    saveSearchQuery();
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
    FilterLineEdit *edit = qobject_cast<FilterLineEdit*>(sender()->parent());

    if(edit) {
        edit->setText(Utils::CLucene::clearSpecialChars(edit->text()));
    }
}
