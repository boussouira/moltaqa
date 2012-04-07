#include "authorsview.h"
#include "ui_authorsview.h"
#include "authorsmanager.h"
#include "librarybookmanager.h"
#include "modelviewfilter.h"
#include "modelenums.h"
#include "utils.h"
#include "webview.h"
#include "htmlhelper.h"

static AuthorsView *m_instance = 0;

AuthorsView::AuthorsView(QWidget *parent) :
    AbstarctView(parent),
    ui(new Ui::AuthorsView),
    m_model(0)
{
    ui->setupUi(this);

    m_instance = this;

    ui->tabWidget->setAutoTabClose(true);

    m_manager = AuthorsManager::instance();
    m_filter = new ModelViewFilter(this);

    connect(ui->tabWidget, SIGNAL(lastTabClosed()), SIGNAL(hideMe()));
}

AuthorsView::~AuthorsView()
{
    ML_DELETE_CHECK(m_model);
    ML_DELETE_CHECK(m_filter);

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
}

void AuthorsView::openAuthorInfo(int authorID)
{
    AuthorInfoPtr info = m_manager->getAuthorInfo(authorID);
    ML_ASSERT2(info, "AuthorsView::openAuthorInfo no author with id" << authorID);

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
    QDir styleDir(App::stylesDir());
    styleDir.cd("default");

    QString style = QUrl::fromLocalFile(styleDir.filePath("default.css")).toString();
    QList<LibraryBookPtr> books = LibraryBookManager::instance()->getAuthorBooks(info->id);

    HtmlHelper html;
    html.beginHtml();
    html.beginHead();
    html.setCharset();
    html.addCSS(style);
    html.setTitle(info->name);
    html.endHead();

    html.beginBody();

    html.beginDivTag(".rawi-info");

    html.beginDivTag(".nav");

    if(!books.isEmpty())
    html.insertLinkTag(tr("الكتب "), "#books");

    if(!info->info.isEmpty())
        html.insertLinkTag(tr("الترجمة"), "#tarejama");

    html.endDivTag(); // .nav

    html.beginDivTag("#info");

    if(!info->name.isEmpty()) {
        html.beginParagraphTag();
        html.insertSpanTag(tr("الاسم: "), ".pro-name");
        html.insertSpanTag(info->name, ".pro-value");
        html.endParagraphTag();
    }

    if(!info->fullName.isEmpty()) {
        html.beginParagraphTag();
        html.insertSpanTag(tr("الاسم الكامل: "), ".pro-name");
        html.insertSpanTag(info->fullName, ".pro-value");
        html.endParagraphTag();
    }

    if(!info->birthStr.isEmpty()) {
        html.beginParagraphTag();
        html.insertSpanTag(tr("الولادة: "), ".pro-name");
        html.insertSpanTag(info->birthStr, ".pro-value");
        html.endParagraphTag();
    }

    if(!info->deathStr.isEmpty()) {
        html.beginParagraphTag();
        html.insertSpanTag(tr("الوفاة: "), ".pro-name");
        html.insertSpanTag(info->deathStr, ".pro-value");
        html.endParagraphTag();
    }

    html.endDivTag(); // #info


    if(!books.isEmpty()) {
        html.insertHeadTag(4, tr("كتب المؤلف"), "#books");
        HtmlHelper bookHtml;

        for(int i=0; i<books.size(); i++) {
            if(i)
                bookHtml.insertBr();

            bookHtml.beginHtmlTag("a", "",
                                  QString("href='moltaqa://open/book?id=%1'").arg(books[i]->bookID));
            bookHtml.append(books[i]->bookDisplayName);
            bookHtml.endHtmlTag();

        }

        html.insertDivTag(bookHtml.html(), ".head-info");
    }

    if(!info->info.isEmpty()) {
        html.insertHeadTag(4, tr("الترجمة"), "#tarejama");
        html.insertDivTag(info->info, ".head-info");
    }

    html.endDivTag(); // .rawi-info

    html.addJS("jquery.js");
    html.addJS("jquery.tooltip.js");
    html.addJS("scripts.js");

    html.endAllTags();

    if(m_crtlKey || !ui->tabWidget->count()) {
        int index = addTab();
        ui->tabWidget->setCurrentIndex(index);
    }

    setCurrentTabHtml(info->name, html.html());
}

void AuthorsView::setCurrentTabHtml(QString title, QString html)
{
    int index = ui->tabWidget->currentIndex();
    ML_ASSERT(index != -1);

    QWidget *w = ui->tabWidget->widget(index);
    ML_ASSERT(w);

    WebView *view = w->findChild<WebView*>();
    ML_ASSERT(view);

    view->setHtml(html);

    ui->tabWidget->setTabText(index, Utils::abbreviate(title, 50));
    ui->tabWidget->setTabToolTip(index, title);
}

void AuthorsView::on_treeView_doubleClicked(const QModelIndex &index)
{
    int authID = index.data(ItemRole::authorIdRole).toInt();

    AuthorInfoPtr info = m_manager->getAuthorInfo(authID);
    ML_ASSERT2(info, "AuthorsView::doubleClicked no author with id" << authID);

    setCurrentAuth(info);
}
