#include "searchquerywidget.h"
#include "ui_searchquerywidget.h"
#include "clheader.h"
#include "clutils.h"
#include "clconstants.h"
#include "arabicanalyzer.h"
#include "clucenequery.h"
#include "utils.h"
#include "librarymanager.h"
#include "searchmanager.h"

#include <qmessagebox.h>
#include <qsettings.h>
#include <qstringlistmodel.h>
#include <qcompleter.h>
#include <qmenu.h>

SearchQueryWidget::SearchQueryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchQueryWidget)
{
    ui->setupUi(this);

    m_advancedSearch = false;
    m_completer = 0;

    setupCleanMenu();
    loadSearchQuery();
    updateSearchWidget();

    connect(ui->toolAdvancedSearch,
            SIGNAL(clicked()),
            SLOT(toggoleAdvancedSearchWidget()));

    connect(ui->labelAdvancedSearch,
            SIGNAL(linkActivated(QString)),
            SLOT(toggoleAdvancedSearchWidget()));

    connect(ui->lineDefaultQuery, SIGNAL(returnPressed()), SIGNAL(search()));
    connect(ui->lineAllWordsQuery, SIGNAL(returnPressed()), SIGNAL(search()));
    connect(ui->lineAnyWordQuery, SIGNAL(returnPressed()), SIGNAL(search()));
    connect(ui->lineExactQuery, SIGNAL(returnPressed()), SIGNAL(search()));
    connect(ui->lineWithoutQuery, SIGNAL(returnPressed()), SIGNAL(search()));
}

SearchQueryWidget::~SearchQueryWidget()
{
    delete ui;
}

Query *SearchQueryWidget::searchQuery(const wchar_t *searchField)
{
    try {

        if(m_advancedSearch) {
            return advancedQuery(searchField);
        } else {
            return defaultQuery(searchField);
        }

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

    }
    catch(...) {
        QMessageBox::warning(0,
                             "CLucene Query error",
                             tr("Unknow error"));
    }

    return 0;
}

void SearchQueryWidget::setSearchQuery(const QString &text)
{
    if(m_advancedSearch)
        ui->lineAllWordsQuery->setText(text);
    else
        ui->lineDefaultQuery->setText(text);
}

QString SearchQueryWidget::searchQueryStr(bool clean)
{
    if(clean && m_searchQuery.size() > 2) {
        QString str = m_searchQuery;
        str.remove(0, 1);
        str.remove(str.size()-1, 1);
        str.replace("][", " ");

        return str;
    }

    return m_searchQuery;
}

void SearchQueryWidget::loadSettings()
{
    QSettings settings;

    settings.beginGroup("SearchWidget");

    if(settings.value("saveSearchOptions", true).toBool()) {
        m_advancedSearch = settings.value("advancedSearch", false).toBool();

        ui->checkDefaultQuery->setChecked(settings.value("checkDefaultQuery", false).toBool());
        ui->checkWithoutQuery->setChecked(settings.value("checkWithoutQuery", false).toBool());

        updateSearchWidget();
    }
}

void SearchQueryWidget::saveSettings()
{
    QSettings settings;

    settings.beginGroup("SearchWidget");

    settings.setValue("advancedSearch", m_advancedSearch);
    settings.setValue("checkDefaultQuery", ui->checkDefaultQuery->isChecked());
    settings.setValue("checkWithoutQuery", ui->checkWithoutQuery->isChecked());
}

void SearchQueryWidget::loadSearchQuery()
{
    ml_return_on_fail(Utils::Settings::get("Search/saveSearch", true).toBool());

    m_completerModel = LibraryManager::instance()->searchManager()->getSavedSearchModel();

    if(!m_completer) {
        m_completer = new QCompleter(m_completerModel, this);
        ui->lineDefaultQuery->setCompleter(m_completer);
        ui->lineAllWordsQuery->setCompleter(m_completer);
        ui->lineAnyWordQuery->setCompleter(m_completer);
        ui->lineExactQuery->setCompleter(m_completer);
        ui->lineWithoutQuery->setCompleter(m_completer);
    }
}

void SearchQueryWidget::saveSearchQuery()
{
    ml_return_on_fail(Utils::Settings::get("Search/saveSearch", true).toBool());

    QStringList list;
    if(m_advancedSearch) {
        list << ui->lineAllWordsQuery->text().trimmed()
             << ui->lineAnyWordQuery->text().trimmed()
             << ui->lineExactQuery->text().trimmed()
             << ui->lineWithoutQuery->text().trimmed();
    } else {
        list << ui->lineDefaultQuery->text().trimmed();
    }

    LibraryManager::instance()->searchManager()->saveSearchQueries(list);

    QStringList words = m_completerModel->stringList();
    if(m_completerModel) {
        foreach (QString q, list) {
            if(q.size() && !words.contains(q))
                words.append(q);
        }

        m_completerModel->setStringList(words);
    }
}

Query *SearchQueryWidget::defaultQuery(const wchar_t *searchField)
{
    QString qureyStr = ui->lineDefaultQuery->text().trimmed();

    if(qureyStr.isEmpty()){
            QMessageBox::warning(this,
                                 tr("البحث"),
                                 tr("لم تدخل أي كلمة ليتم البحث عنها"));
            return 0;
    }

    ArabicAnalyzer analyzer;
    QueryParser queryPareser(searchField, &analyzer);
    queryPareser.setAllowLeadingWildcard(true);

    m_searchQuery = QString("[%1]").arg(qureyStr);

    return Utils::CLucene::parse(&queryPareser,
                                 qureyStr,
                                 ui->checkDefaultQuery->isChecked());
}

Query *SearchQueryWidget::advancedQuery(const wchar_t *searchField)
{
    if(ui->lineAllWordsQuery->text().trimmed().isEmpty()
            && ui->lineAnyWordQuery->text().trimmed().isEmpty()
            && ui->lineExactQuery->text().trimmed().isEmpty()) {
        QMessageBox::warning(this,
                             tr("البحث"),
                             tr("لم تدخل أي كلمة ليتم البحث عنها"));
        return 0;
    }

    ArabicAnalyzer analyzer;
    QueryParser queryPareser(searchField, &analyzer);
    queryPareser.setAllowLeadingWildcard(true);

    // All words query
    Query *allWordsQuery = Utils::CLucene::parse(&queryPareser,
                                                 ui->lineAllWordsQuery->text().trimmed(),
                                                 true);

    Query *anyWordQuery = Utils::CLucene::parse(&queryPareser,
                                                ui->lineAnyWordQuery->text().trimmed(),
                                                false);

    Query *exactQuery = 0;
    if(ui->lineExactQuery->text().trimmed().size()) {
        // The query parser do a lot of stuff for phrase query
        QString exactQueryText = QString("\"%1\"").arg(Utils::CLucene::clearSpecialChars(ui->lineExactQuery->text().trimmed()));

        if(ui->spinExactQuerySlop->value())
            exactQueryText.append(QString("~%1").arg(ui->spinExactQuerySlop->value()));

        exactQuery = Utils::CLucene::parse(&queryPareser, exactQueryText, true);
    }

    Query *withoutQuery = Utils::CLucene::parse(&queryPareser,
                                                ui->lineWithoutQuery->text().trimmed(),
                                                ui->checkWithoutQuery->isChecked());

    ml_return_val_on_fail(allWordsQuery || anyWordQuery || exactQuery, 0);

    BooleanQuery *query = new BooleanQuery;

    if(allWordsQuery)
        query->add(allWordsQuery, BooleanClause::MUST);

    if(anyWordQuery)
        query->add(anyWordQuery, BooleanClause::MUST);

    if(exactQuery)
        query->add(exactQuery, BooleanClause::MUST);

    if(withoutQuery)
        query->add(withoutQuery, BooleanClause::MUST_NOT);

    m_searchQuery = QString("[%1][%2][%3][%4]")
            .arg(ui->lineAllWordsQuery->text().trimmed())
            .arg(ui->lineAnyWordQuery->text().trimmed())
            .arg(ui->lineExactQuery->text().trimmed())
            .arg(ui->lineWithoutQuery->text().trimmed());

    return query;
}

void SearchQueryWidget::toggoleAdvancedSearchWidget()
{
    m_advancedSearch = !m_advancedSearch;
    updateSearchWidget();
}

void SearchQueryWidget::setupCleanMenu()
{
    QList<FilterLineEdit*> lines;
    lines << ui->lineDefaultQuery
          << ui->lineAllWordsQuery
          << ui->lineAnyWordQuery
          << ui->lineExactQuery
          << ui->lineWithoutQuery;

    foreach(FilterLineEdit *line, lines) {
        QMenu *menu = new QMenu(line);
        QAction *clearSpecialCharAct = new QAction(tr("ابطال مفعول الاقواس وغيرها"), line);
        QAction *removeTashekilAct = new QAction(tr("حذف التشكيل"), line);

        if(line != ui->lineExactQuery) {
            menu->addAction(clearSpecialCharAct);
            menu->addSeparator();
        }

        menu->addAction(removeTashekilAct);

        connect(clearSpecialCharAct, SIGNAL(triggered()), SLOT(clearSpecialChar()));
        connect(removeTashekilAct, SIGNAL(triggered()), SLOT(removeTashekil()));

        line->setFilterMenu(menu);
    }
}

void SearchQueryWidget::clearSpecialChar()
{
    FilterLineEdit *edit = qobject_cast<FilterLineEdit*>(sender()->parent());

    if(edit) {
        edit->setText(Utils::CLucene::clearSpecialChars(edit->text()));
    }
}

void SearchQueryWidget::matchSearch()
{
    FilterLineEdit *edit = qobject_cast<FilterLineEdit*>(sender()->parent());

    if(edit) {
        QString text = edit->text().trimmed();
        if(text.isEmpty())
            return;

        QChar del('"');
        if(text.startsWith(del) && text.endsWith(del)) {
            text = text.remove(0, 1).remove(text.size()-2, 1);
        } else {
            if(!text.startsWith(del))
                text.prepend(del);

            if(!text.endsWith(del))
                text.append(del);
        }

        edit->setText(text);
    }
}

void SearchQueryWidget::removeTashekil()
{
    FilterLineEdit *edit = qobject_cast<FilterLineEdit*>(sender()->parent());

    if(edit) {
        edit->setText(edit->text().remove(QRegExp("[\\x064B-\\x0653]")).trimmed());
    }
}

void SearchQueryWidget::updateSearchWidget()
{
    ui->widgetAdvancedSearch->setVisible(m_advancedSearch);
    ui->toolAdvancedSearch->setText((m_advancedSearch ? "-" : "+"));
    ui->lineDefaultQuery->setVisible(!m_advancedSearch);
    ui->checkDefaultQuery->setVisible(!m_advancedSearch);
    ui->labelDefaultQuery->setVisible(!m_advancedSearch);
}
