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

    setCurrentWidget(Search);
    loadSettings();

    connect(ui->searchQueryWidget, SIGNAL(search()), SLOT(search()));
    connect(ui->pushSearch, SIGNAL(clicked()), SLOT(search()));
    connect(ui->labelTools, SIGNAL(linkActivated(QString)),
            SLOT(showFilterTools()));
    connect(ui->labelSearchField, SIGNAL(linkActivated(QString)),
            SLOT(showSearchFieldMenu()));
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

void SearchWidget::setSearchText(QString query)
{
    ui->searchQueryWidget->setSearchQuery(Utils::CLucene::clearSpecialChars(query.trimmed()));
}

Query *SearchWidget::getSearchQuery(const wchar_t *searchField)
{
    return ui->searchQueryWidget->searchQuery(searchField);
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

        ui->checkShowPageInfo->setChecked(settings.value("showPageInfo", true).toBool());
        ui->checkShowResultTitles->setChecked(settings.value("showResultTitles", true).toBool());
    }

    ui->searchQueryWidget->loadSettings();
}

void SearchWidget::saveSettings()
{
    QSettings settings;

    settings.beginGroup("SearchWidget");

    settings.setValue("sortSearch", ui->comboSortSearch->currentIndex());
    settings.setValue("searchField", ui->comboSearchField->currentIndex());

    settings.setValue("showPageInfo", ui->checkShowPageInfo->isChecked());
    settings.setValue("showResultTitles", ui->checkShowResultTitles->isChecked());

    ui->searchQueryWidget->saveSettings();
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

    ui->searchQueryWidget->saveSearchQuery();

    QString log = QString("query: %1, take %2 ms, results count %3")
            .arg(ui->searchQueryWidget->searchQueryStr().trimmed())
            .arg(m_searcher ? m_searcher->searchTime() : -1)
            .arg(m_searcher ? m_searcher->resultsCount() : -1);

    StatisticsManager::instance()->add("search", log);

    if(m_searcher
            && Utils::Settings::get("Search/returnToSearchWidget", true).toBool()
            && !m_searcher->resultsCount()) {
        setCurrentWidget(Search);
        QMessageBox::warning(this,
                             tr("بحث"),
                             tr("لم يتم العثور على ما يطابق بحثك"));
    }
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
