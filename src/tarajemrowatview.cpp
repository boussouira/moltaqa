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

    ml_set_instance(m_instance, this);
    ui->tabWidget->setAutoTabClose(true);

    m_filter = new ModelViewFilter(this);
    m_rowatManager = LibraryManager::instance()->rowatManager();

    Q_CHECK_PTR(m_rowatManager);

    connect(ui->tabWidget, SIGNAL(lastTabClosed()), SLOT(lastTabClosed()));
}

TarajemRowatView::~TarajemRowatView()
{
    ml_delete_check(m_model);

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
    ml_return_val_on_fail(m_currentRawi, QString());

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

    if(Utils::Settings::contains("RowatView/splitter"))
        ui->splitter->restoreState(Utils::Settings::get("RowatView/splitter").toByteArray());

    if(!m_currentRawi) {
        if(!openRawiInfo(Utils::Settings::get("RowatView/last").toInt())) {
            on_treeView_doubleClicked(m_model->index(0, 0));
        }
    }
}

void TarajemRowatView::aboutToHide()
{
    Utils::Settings::set("RowatView/splitter", ui->splitter->saveState());
}

bool TarajemRowatView::openRawiInfo(int rawiID)
{
    RawiInfo::Ptr info = m_rowatManager->getRawiInfo(rawiID);
    ml_return_val_on_fail2(info, "TarajemRowatView::openRawiInfo no rawi with id" << rawiID, false);

    setCurrentRawi(info);

    emit showMe();

    return true;
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

void TarajemRowatView::setCurrentRawi(RawiInfo::Ptr info)
{
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
    html.insertLink(tr("الشيوخ "), "#sheok");
    html.insertLink(tr("التلاميذ "), "#talamid");
    html.insertLink(tr("الترجمة"), "#tarejama");
    html.endDiv();

    html.beginDiv("#info");
    html.beginDL(".dl-horizontal");

    html.insertDT(tr("اسم الراوي: "));
    html.insertDD(info->name);

    if(info->laqab.size()) {
        html.insertDT(tr("اللقب: "));
        html.insertDD(info->laqab);
    }

    if(info->birthStr.size()) {
        html.insertDT(tr("الولادة: "));
        html.insertDD(info->birthStr);
    }

    if(info->deathStr.size()) {
        html.insertDT(tr("الوفاة: "));
        html.insertDD(info->deathStr);
    }

    html.insertDT(tr("الطبقة: "));
    html.insertDD(info->tabaqa);

    html.insertDT(tr("روى له: "));
    html.insertDD(info->rowat);

    html.insertDT(tr("رتبته عند ابن حجر: "));
    html.insertDD(info->rotba_hafed);

    html.insertDT(tr("رتبته عند الذهبي: "));
    html.insertDD(info->rotba_zahabi);

    html.endDL(); // .dl-horizontal
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
    ml_return_on_fail(index != -1);

    QWidget *w = ui->tabWidget->widget(index);
    ml_return_on_fail(w);

    QWebView *view = w->findChild<QWebView*>();
    ml_return_on_fail(view);

    view->setHtml(html);

    ui->tabWidget->setTabText(index, Utils::String::abbreviate(title, 50));
    ui->tabWidget->setTabToolTip(index, title);
}

void TarajemRowatView::on_treeView_doubleClicked(const QModelIndex &index)
{
    int rawiID = index.data(ItemRole::authorIdRole).toInt();
    ml_return_on_fail(rawiID);

    RawiInfo::Ptr info = m_rowatManager->getRawiInfo(rawiID);
    ml_return_on_fail(info);

    setCurrentRawi(info);
}

void TarajemRowatView::lastTabClosed()
{
    if(m_currentRawi) {
        Utils::Settings::set("RowatView/last", m_currentRawi->id);

        m_currentRawi.clear();
    }

    emit hideMe();
}
