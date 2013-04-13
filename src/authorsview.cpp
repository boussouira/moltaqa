#include "authorsview.h"
#include "ui_authorsview.h"
#include "authorsmanager.h"
#include "htmlhelper.h"
#include "librarybookmanager.h"
#include "librarymanager.h"
#include "modelenums.h"
#include "modelviewfilter.h"
#include "stringutils.h"
#include "utils.h"
#include "webview.h"

static AuthorsView *m_instance = 0;

AuthorsView::AuthorsView(QWidget *parent) :
    AbstarctView(parent),
    ui(new Ui::AuthorsView),
    m_model(0)
{
    ui->setupUi(this);

    ml_set_instance(m_instance, this);

    ui->tabWidget->setAutoTabClose(true);

    m_manager = LibraryManager::instance()->authorsManager();
    m_bookManager = LibraryManager::instance()->bookManager();

    m_filter = new ModelViewFilter(this);

    connect(ui->tabWidget, SIGNAL(lastTabClosed()), SLOT(lastTabClosed()));
}

AuthorsView::~AuthorsView()
{
    ml_delete_check(m_model);
    ml_delete_check(m_filter);

    delete ui;

    m_instance = 0;
}

AuthorsView *AuthorsView::instance()
{
    return m_instance;
}

QString AuthorsView::title()
{
    return tr("تراجم المؤلفين");
}

QString AuthorsView::viewLink()
{
    ml_return_val_on_fail(m_currentAuthor, QString());

    return QString("moltaqa://?c=open&t=author&id=%1").arg(m_currentAuthor->id);
}

void AuthorsView::aboutToShow()
{
    if(!m_model) {
        m_model = m_manager->authorsModel();

        m_filter->setSourceModel(m_model);
        m_filter->setLineEdit(ui->lineEdit);
        m_filter->setTreeView(ui->treeView);
        m_filter->setup();
    }

    if(!ui->tabWidget->count())
        addTab(tr("المؤلف"));

    if(Utils::Settings::contains("AuthorsView/splitter"))
        ui->splitter->restoreState(Utils::Settings::get("AuthorsView/splitter").toByteArray());

    if(!m_currentAuthor) {
        if(!openAuthorInfo(Utils::Settings::get("AuthorsView/last").toInt())) {
            on_treeView_doubleClicked(m_model->index(0, 0));
        }
    }
}

void AuthorsView::aboutToHide()
{
    Utils::Settings::set("AuthorsView/splitter", ui->splitter->saveState());
}

bool AuthorsView::openAuthorInfo(int authorID)
{
    AuthorInfo::Ptr info = m_manager->getAuthorInfo(authorID);
    ml_return_val_on_fail2(info, "AuthorsView::openAuthorInfo no author with id" << authorID, false);

    setCurrentAuth(info);

    emit showMe();

    return true;
}

int AuthorsView::addTab(QString tabText)
{
    QWidget *tabAuthInfo = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tabAuthInfo);
    layout->setContentsMargins(0, 0, 0, 0);
    WebView *webAuthInfo = new WebView(tabAuthInfo);
    webAuthInfo->setUrl(QUrl("about:blank"));

    layout->addWidget(webAuthInfo);

    return ui->tabWidget->addTab(tabAuthInfo, tabText);
}

void AuthorsView::setCurrentAuth(AuthorInfo::Ptr info)
{
    QList<LibraryBook::Ptr> books = m_bookManager->getAuthorBooks(info->id);

    HtmlHelper html;
    html.beginHtml();
    html.beginHead();
    html.setCharset();
    html.addCSS("default.css");
    html.setTitle(info->name);
    html.endHead();

    html.beginBody();

    html.beginDiv(".rawi-info");

    html.beginDiv(".nav");

    if(books.size())
    html.insertLink(tr("الكتب "), "#books");

    if(info->info.size())
        html.insertLink(tr("الترجمة"), "#tarejama");

    html.endDiv(); // .nav

    html.beginDiv("#info");
    html.beginDL(".dl-horizontal");

    if(info->name.size()) {
        html.insertDT(tr("الاسم: "));

        if(info->isALive)
            html.insertDD(tr("%1 (معاصر)").arg(info->name));
        else
            html.insertDD(info->name);

    }

    if(info->fullName.size()) {
        html.insertDT(tr("الاسم الكامل: "));
        html.insertDD(info->fullName);
    }

    if(info->birthStr.size()) {
        html.insertDT(tr("الولادة: "));
        html.insertDD(info->birthStr);
    }

    if(info->deathStr.size()) {
        html.insertDT(tr("الوفاة: "));
        html.insertDD(info->deathStr);
    }

    html.endDL(); // .dl-horizontal
    html.endDiv(); // #info

    if(books.size()) {
        html.insertHead(4, tr("كتب المؤلف (%1)").arg(books.size()), "#books");
        HtmlHelper bookHtml;

        for(int i=0; i<books.size(); i++) {
            if(i)
                bookHtml.insertBr();

            bookHtml.beginHtmlTag("a", "",
                                  QString("href='moltaqa://?c=open&t=book&id=%1'").arg(books[i]->uuid));
            bookHtml.appendText(books[i]->title);
            bookHtml.endHtmlTag();

        }

        html.insertDiv(bookHtml.html(), ".head-info");
    }

    if(info->info.size()) {
        html.insertHead(4, tr("الترجمة"), "#tarejama");
        html.insertDiv(info->info, ".head-info");
    }

    html.endDiv(); // .rawi-info

    html.addJS("jquery.js");
    html.addJS("jquery.tooltip.js");
    html.addJS("scripts.js");

    html.addJSCode("setupToolTip();");

    html.endAll();

    if(m_crtlKey || !ui->tabWidget->count()) {
        int index = addTab();
        ui->tabWidget->setCurrentIndex(index);
    }

    m_currentAuthor = info;

    setCurrentTabHtml(info->name, html.html());
}

void AuthorsView::setCurrentTabHtml(QString title, QString html)
{
    int index = ui->tabWidget->currentIndex();
    ml_return_on_fail(index != -1);

    QWidget *w = ui->tabWidget->widget(index);
    ml_return_on_fail(w);

    WebView *view = w->findChild<WebView*>();
    ml_return_on_fail(view);

    view->setHtml(html);

    ui->tabWidget->setTabText(index, Utils::String::abbreviate(title, 50));
    ui->tabWidget->setTabToolTip(index, title);
}

void AuthorsView::on_treeView_doubleClicked(const QModelIndex &index)
{
    int authID = index.data(ItemRole::authorIdRole).toInt();

    AuthorInfo::Ptr info = m_manager->getAuthorInfo(authID);
    ml_return_on_fail2(info, "AuthorsView::doubleClicked no author with id" << authID);

    setCurrentAuth(info);
}

void AuthorsView::lastTabClosed()
{
    if(m_currentAuthor) {
        Utils::Settings::set("AuthorsView/last", m_currentAuthor->id);

        m_currentAuthor.clear();
    }

    emit hideMe();
}
