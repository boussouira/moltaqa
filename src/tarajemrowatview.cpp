#include "tarajemrowatview.h"
#include "ui_tarajemrowatview.h"
#include "tarajemrowatmanager.h"
#include "modelenums.h"
#include "htmlhelper.h"
#include "utils.h"
#include "stringutils.h"
#include "modelviewfilter.h"
#include <qstandarditemmodel.h>
#include <qdir.h>
#include <qurl.h>
#include <qdebug.h>
#include <qwebview.h>

TarajemRowatView::TarajemRowatView(QWidget *parent) :
    AbstarctView(parent),
    m_model(0),
    m_filter(0),
    ui(new Ui::TarajemRowatView)
{
    ui->setupUi(this);

    ui->tabWidget->setAutoTabClose(true);
    ui->tabWidget->setCloseLastTab(false);

    m_filter = new ModelViewFilter(this);
    m_rowatManager = TarajemRowatManager::instance();

    addTab(tr("الراوي"));

    Q_CHECK_PTR(m_rowatManager);
}

TarajemRowatView::~TarajemRowatView()
{
    ML_DELETE_CHECK(m_model);

    delete ui;
}

QString TarajemRowatView::title()
{
    return tr("تراجم الرواة");
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

void TarajemRowatView::setCurrentRawi(RawiInfo *info)
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

    html.insertHtmlTag("h3", tr("الشيوخ"), "#sheok");
    html.insertDivTag(Utils::formatHTML(info->sheok), ".head-info");

    html.insertHtmlTag("h3", tr("التلاميذ"), "#talamid");
    html.insertDivTag(Utils::formatHTML(info->talamid), ".head-info");

    html.insertHtmlTag("h3", tr("الترجمة"), "#tarejama");
    html.insertDivTag(Utils::formatHTML(info->tarejama), ".head-info");

    html.endAllTags();

    if(m_crtlKey || !ui->tabWidget->count()) {
        int index = addTab();
        ui->tabWidget->setCurrentIndex(index);
    }

    setCurrentTabHtml(info->name, html.html());
}

void TarajemRowatView::setCurrentTabHtml(QString title, QString html)
{
    int index = ui->tabWidget->currentIndex();
    ML_RETURN(index == -1);

    QWidget *w = ui->tabWidget->widget(index);
    ML_RETURN(!w);

    QWebView *view = w->findChild<QWebView*>();
    ML_RETURN(!view);

    view->setHtml(html);

    ui->tabWidget->setTabText(index, Utils::abbreviate(title, 50));
    ui->tabWidget->setTabToolTip(index, title);
}

void TarajemRowatView::on_treeView_doubleClicked(const QModelIndex &index)
{
    int rawiID = index.data(ItemRole::authorIdRole).toInt();
    ML_RETURN(!rawiID);

    RawiInfo *info = m_rowatManager->getRawiInfo(rawiID);
    ML_RETURN(!info);

    setCurrentRawi(info);
}
