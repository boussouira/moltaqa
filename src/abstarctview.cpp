#include "abstarctview.h"
#include "webviewsearcher.h"
#include "filterlineedit.h"
#include "utils.h"
#include <qaction.h>
#include <qtoolbar.h>

AbstarctView::AbstarctView(QWidget *parent) :
    QWidget(parent),
    m_selectable(true),
    m_crtlKey(false)
{
    // Search toolbar
    m_toolBarSearch = new QToolBar(tr("البحث"), 0);

    // Search line
    m_searchEdit = new FilterLineEdit(m_toolBarSearch);
    m_searchEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    m_searchEdit->setToolTip(tr("بحث في الصفحة"));

    // Search buttons
    m_searchPrevAction =  new QAction(ml_theme_icon("go-up-search", ":/images/go-up-search.png"),
                                      tr("السابق"),
                                      m_toolBarSearch);
    m_searchNextAction =  new QAction(ml_theme_icon("go-down-search", ":/images/go-down-search.png"),
                                      tr("التالي"),
                                      m_toolBarSearch);

    m_searchNextAction->setVisible(false);
    m_searchPrevAction->setVisible(false);

    m_toolBarSearch->addWidget(m_searchEdit);
    m_toolBarSearch->addAction(m_searchNextAction);
    m_toolBarSearch->addAction(m_searchPrevAction);

    connect(m_searchEdit, SIGNAL(delayFilterChanged()), SLOT(searchInPage()));
    connect(m_searchNextAction, SIGNAL(triggered()), SLOT(searchNext()));
    connect(m_searchPrevAction, SIGNAL(triggered()), SLOT(searchPrev()));
}

AbstarctView::~AbstarctView()
{
}

QList<QToolBar*> AbstarctView::toolBars()
{
    return m_toolBars;
}

QList<QAction *> AbstarctView::navigationActions()
{
    return m_navActions;
}

void AbstarctView::updateToolBars()
{
}

void AbstarctView::updateActions()
{
}

void AbstarctView::aboutToShow()
{
}

void AbstarctView::aboutToHide()
{
}

bool AbstarctView::isSelectable()
{
    return m_selectable;
}

void AbstarctView::setSelectable(bool selectebale)
{
    m_selectable = selectebale;
}

QAction *AbstarctView::actionSeparator(QObject *parent)
{
    QAction *act = new QAction(parent);
    act->setSeparator(true);

    return act;
}

QString AbstarctView::viewLink()
{
    return QString();
}

WebViewSearcher *AbstarctView::searcher()
{
    return 0;
}

void AbstarctView::keyPressEvent(QKeyEvent *event)
{
    if(!m_crtlKey && (event->modifiers() & Qt::ControlModifier))
        m_crtlKey = true;
}

void AbstarctView::keyReleaseEvent(QKeyEvent *event)
{
    if(m_crtlKey && !(event->modifiers() & Qt::ControlModifier))
        m_crtlKey = false;
}

void AbstarctView::searchInPage()
{
    WebViewSearcher *search = searcher();
    ml_return_on_fail(search);

    QString searchText = m_searchEdit->text().trimmed();
    bool hasResult = search->search(searchText);

    QString bg = ((searchText.isEmpty() || hasResult) ? "#FFFFFF" : "#F2DEDE");
    m_searchEdit->setStyleSheet(QString("background-color: %1").arg(bg));

    updateSearchNavigation();
}

void AbstarctView::searchNext()
{
    WebViewSearcher *search = searcher();
    ml_return_on_fail(search);

    search->next();
    updateSearchNavigation();
}

void AbstarctView::searchPrev()
{
    WebViewSearcher *search = searcher();
    ml_return_on_fail(search);

    search->previous();
    updateSearchNavigation();
}

void AbstarctView::updateSearchNavigation()
{
    WebViewSearcher *search = searcher();
    ml_return_on_fail(search);

    m_searchNextAction->setVisible(search->hasSearchResult());
    m_searchPrevAction->setVisible(search->hasSearchResult());
}
