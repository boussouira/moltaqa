#include "authorsview.h"
#include "ui_authorsview.h"
#include "authorsmanager.h"
#include "librarybookmanager.h"
#include "librarymanager.h"
#include "modelviewfilter.h"
#include "modelenums.h"
#include "utils.h"
#include "stringutils.h"
#include "webview.h"
#include "htmlhelper.h"
#include <qsettings.h>

static AuthorsView *m_instance = 0;

AuthorsView::AuthorsView(QWidget *parent) :
    AbstarctView(parent),
    ui(new Ui::AuthorsView),
    m_model(0)
{
    ui->setupUi(this);

    m_instance = this;

    ui->tabWidget->setAutoTabClose(true);

    m_manager = LibraryManager::instance()->authorsManager();
    m_bookManager = LibraryManager::instance()->bookManager();

    m_filter = new ModelViewFilter(this);

    connect(ui->tabWidget, SIGNAL(lastTabClosed()), SIGNAL(hideMe()));
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
    return tr("المؤلفين");
}

QString AuthorsView::viewLink()
{
    ml_return_val_on_fail(m_currentAuthor, QString());

    return QString("moltaqa://open/author?id=%1").arg(m_currentAuthor->id);
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

    QSettings settings;
    if(settings.contains("AuthorsView/splitter"))
        ui->splitter->restoreState(settings.value("AuthorsView/splitter").toByteArray());
}

void AuthorsView::aboutToHide()
{
    QSettings settings;
    settings.setValue("AuthorsView/splitter", ui->splitter->saveState());
}

void AuthorsView::openAuthorInfo(int authorID)
{
    AuthorInfoPtr info = m_manager->getAuthorInfo(authorID);
    ml_return_on_fail2(info, "AuthorsView::openAuthorInfo no author with id" << authorID);

    setCurrentAuth(info);

    emit showMe();
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

void AuthorsView::setCurrentAuth(AuthorInfoPtr info)
{
    QList<LibraryBookPtr> books = m_bookManager->getAuthorBooks(info->id);

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

    if(info->name.size()) {
        html.beginParagraph();
        html.insertSpan(tr("الاسم: "), ".pro-name");

        if(info->isALive)
            html.insertSpan(tr("%1 (معاصر)").arg(info->name), ".pro-value");
        else
            html.insertSpan(info->name, ".pro-value");

        html.endParagraph();
    }

    if(info->fullName.size()) {
        html.beginParagraph();
        html.insertSpan(tr("الاسم الكامل: "), ".pro-name");
        html.insertSpan(info->fullName, ".pro-value");
        html.endParagraph();
    }

    if(info->birthStr.size()) {
        html.beginParagraph();
        html.insertSpan(tr("الولادة: "), ".pro-name");
        html.insertSpan(info->birthStr, ".pro-value");
        html.endParagraph();
    }

    if(info->deathStr.size()) {
        html.beginParagraph();
        html.insertSpan(tr("الوفاة: "), ".pro-name");
        html.insertSpan(info->deathStr, ".pro-value");
        html.endParagraph();
    }

    html.endDiv(); // #info


    if(books.size()) {
        html.insertHead(4, tr("كتب المؤلف (%1)").arg(books.size()), "#books");
        HtmlHelper bookHtml;

        for(int i=0; i<books.size(); i++) {
            if(i)
                bookHtml.insertBr();

            bookHtml.beginHtmlTag("a", "",
                                  QString("href='moltaqa://open/book?id=%1'").arg(books[i]->id));
            bookHtml.append(books[i]->title);
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

    html.addJSCode("setupToolTip();"
                   "moltaqaLink();");

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

    AuthorInfoPtr info = m_manager->getAuthorInfo(authID);
    ml_return_on_fail2(info, "AuthorsView::doubleClicked no author with id" << authID);

    setCurrentAuth(info);
}
