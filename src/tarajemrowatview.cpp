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

    html.beginDiv(".rawi-info");

    html.beginDiv(".nav");
    html.insertLink(tr("الشيوخ "), "#sheok");
    html.insertLink(tr("التلاميذ "), "#talamid");
    html.insertLink(tr("الترجمة"), "#tarejama");
    html.endDiv();

    html.beginDiv("#info");

    html.beginParagraph();
    html.insertSpan(tr("اسم الراوي: "), ".pro-name");
    html.insertSpan(info->name, ".pro-value");
    html.endParagraph();

    if(!info->laqab.isEmpty()) {
        html.beginParagraph();
        html.insertSpan(tr("اللقب: "), ".pro-name");
        html.insertSpan(info->laqab, ".pro-value");
        html.endParagraph();
    }

    if(!info->birthStr.isEmpty()) {
        html.beginParagraph();
        html.insertSpan(tr("الولادة: "), ".pro-name");
        html.insertSpan(info->birthStr, ".pro-value");
        html.endParagraph();
    }

    if(!info->deathStr.isEmpty()) {
        html.beginParagraph();
        html.insertSpan(tr("الوفاة: "), ".pro-name");
        html.insertSpan(info->deathStr, ".pro-value");
        html.endParagraph();
    }

    html.beginParagraph();
    html.insertSpan(tr("الطبقة: "), ".pro-name");
    html.insertSpan(info->tabaqa, ".pro-value");
    html.endParagraph();

    html.beginParagraph();
    html.insertSpan(tr("روى له: "), ".pro-name");
    html.insertSpan(info->rowat, ".pro-value");
    html.endParagraph();

    html.beginParagraph();
    html.insertSpan(tr("رتبته عند ابن حجر: "), ".pro-name");
    html.insertSpan(info->rotba_hafed, ".pro-value");
    html.endParagraph();

    html.beginParagraph();
    html.insertSpan(tr("رتبته عند الذهبي: "), ".pro-name");
    html.insertSpan(info->rotba_zahabi, ".pro-value");
    html.endParagraph();

    html.endDiv(); // #info

    html.insertHead(4, tr("الشيوخ"), "#sheok");
    html.insertDiv(info->sheok, ".head-info");

    html.insertHead(4, tr("التلاميذ"), "#talamid");
    html.insertDiv(info->talamid, ".head-info");

    html.insertHead(4, tr("الترجمة"), "#tarejama");
    html.insertDiv(info->tarejama, ".head-info");

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
