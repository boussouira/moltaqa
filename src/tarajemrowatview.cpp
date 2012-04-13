#include "tarajemrowatview.h"
#include "ui_tarajemrowatview.h"
#include "tarajemrowatmanager.h"
#include "modelenums.h"
#include "htmlhelper.h"
#include "librarymanager.h"
#include "utils.h"
#include "stringutils.h"
#include "modelviewfilter.h"
#include <qstandarditemmodel.h>
#include <qdir.h>
#include <qurl.h>
#include <qdebug.h>
#include <qwebview.h>

static TarajemRowatView *m_instance = 0;

TarajemRowatView::TarajemRowatView(QWidget *parent) :
    AbstarctView(parent),
    m_model(0),
    m_filter(0),
    ui(new Ui::TarajemRowatView)
{
    ui->setupUi(this);

    m_instance = this;
    ui->tabWidget->setAutoTabClose(true);

    m_filter = new ModelViewFilter(this);
    m_rowatManager = LibraryManager::instance()->rowatManager();

    Q_CHECK_PTR(m_rowatManager);

    connect(ui->tabWidget, SIGNAL(lastTabClosed()), SIGNAL(hideMe()));
}

TarajemRowatView::~TarajemRowatView()
{
    ML_DELETE_CHECK(m_model);

    delete ui;

    m_instance = 0;
}

TarajemRowatView *TarajemRowatView::instance()
{
    return m_instance;
}

QString TarajemRowatView::title()
{
    return tr("تراجم الرواة");
}

QString TarajemRowatView::viewLink()
{
    ML_ASSERT_RET(m_currentRawi, QString());

    return QString("moltaqa://open/rawi?id=%1").arg(m_currentRawi->id);
}

void TarajemRowatView::aboutToShow()
{
    if(!m_model) {
        m_model = m_rowatManager->getRowatModel();

        m_filter->setSourceModel(m_model);
        m_filter->setLineEdit(ui->lineFilter);
        m_filter->setTreeView(ui->treeView);
        m_filter->setup();
    }

    if(!ui->tabWidget->count())
        addTab(tr("الراوي"));
}

void TarajemRowatView::openRawiInfo(int rawiID)
{
    RawiInfoPtr info = m_rowatManager->getRawiInfo(rawiID);
    ML_ASSERT2(info, "TarajemRowatView::openRawiInfo no rawi with id" << rawiID);

    setCurrentRawi(info);

    emit showMe();
}

int TarajemRowatView::addTab(QString tabText)
{
    QWidget *tabRawiInfo = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tabRawiInfo);
    layout->setContentsMargins(0, 0, 0, 0);
    QWebView *webRawiInfo = new QWebView(tabRawiInfo);
    webRawiInfo->setUrl(QUrl("about:blank"));

    layout->addWidget(webRawiInfo);

    return ui->tabWidget->addTab(tabRawiInfo, tabText);
}

void TarajemRowatView::setCurrentRawi(RawiInfoPtr info)
{
    QDir styleDir(App::stylesDir());
    styleDir.cd("default");

    QString style = QUrl::fromLocalFile(styleDir.filePath("default.css")).toString();

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
    html.insertLinkTag(tr("الشيوخ "), "#sheok");
    html.insertLinkTag(tr("التلاميذ "), "#talamid");
    html.insertLinkTag(tr("الترجمة"), "#tarejama");
    html.endDivTag();

    html.beginDivTag("#info");

    html.beginParagraphTag();
    html.insertSpanTag(tr("اسم الراوي: "), ".pro-name");
    html.insertSpanTag(info->name, ".pro-value");
    html.endParagraphTag();

    if(!info->laqab.isEmpty()) {
        html.beginParagraphTag();
        html.insertSpanTag(tr("اللقب: "), ".pro-name");
        html.insertSpanTag(info->laqab, ".pro-value");
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

    html.beginParagraphTag();
    html.insertSpanTag(tr("الطبقة: "), ".pro-name");
    html.insertSpanTag(info->tabaqa, ".pro-value");
    html.endParagraphTag();

    html.beginParagraphTag();
    html.insertSpanTag(tr("روى له: "), ".pro-name");
    html.insertSpanTag(info->rowat, ".pro-value");
    html.endParagraphTag();

    html.beginParagraphTag();
    html.insertSpanTag(tr("رتبته عند ابن حجر: "), ".pro-name");
    html.insertSpanTag(info->rotba_hafed, ".pro-value");
    html.endParagraphTag();

    html.beginParagraphTag();
    html.insertSpanTag(tr("رتبته عند الذهبي: "), ".pro-name");
    html.insertSpanTag(info->rotba_zahabi, ".pro-value");
    html.endParagraphTag();

    html.endDivTag(); // #info

    html.insertHeadTag(4, tr("الشيوخ"), "#sheok");
    html.insertDivTag(info->sheok, ".head-info");

    html.insertHeadTag(4, tr("التلاميذ"), "#talamid");
    html.insertDivTag(info->talamid, ".head-info");

    html.insertHeadTag(4, tr("الترجمة"), "#tarejama");
    html.insertDivTag(info->tarejama, ".head-info");

    html.endDivTag(); // .rawi-info

    html.addJS("jquery.js");
    html.addJS("jquery.tooltip.js");
    html.addJS("scripts.js");

    html.addJSCode("setupToolTip();"
                   "moltaqaLink();");

    html.endAllTags();

    if(m_crtlKey || !ui->tabWidget->count()) {
        int index = addTab();
        ui->tabWidget->setCurrentIndex(index);
    }

    m_currentRawi = info;

    setCurrentTabHtml(info->name, html.html());
}

void TarajemRowatView::setCurrentTabHtml(QString title, QString html)
{
    int index = ui->tabWidget->currentIndex();
    ML_ASSERT(index != -1);

    QWidget *w = ui->tabWidget->widget(index);
    ML_ASSERT(w);

    QWebView *view = w->findChild<QWebView*>();
    ML_ASSERT(view);

    view->setHtml(html);

    ui->tabWidget->setTabText(index, Utils::String::abbreviate(title, 50));
    ui->tabWidget->setTabToolTip(index, title);
}

void TarajemRowatView::on_treeView_doubleClicked(const QModelIndex &index)
{
    int rawiID = index.data(ItemRole::authorIdRole).toInt();
    ML_ASSERT(rawiID);

    RawiInfoPtr info = m_rowatManager->getRawiInfo(rawiID);
    ML_ASSERT(info);

    setCurrentRawi(info);
}
