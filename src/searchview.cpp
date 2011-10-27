#include "searchview.h"
#include "tabwidget.h"
#include "searchwidget.h"
#include <qboxlayout.h>

SearchView::SearchView(QWidget *parent) : AbstarctView(parent)
{
    m_layout = new QVBoxLayout(this);
    m_tabWidget = new TabWidget(this);
    m_searchWidget = new SearchWidget(this);

    m_tabWidget->addTab(m_searchWidget,
                        QIcon(":/menu/images/find.png"),
                        tr("بحث"));
    m_tabWidget->setDocumentMode(true);

    m_layout->addWidget(m_tabWidget);
    m_layout->setContentsMargins(0, 6, 0, 0);

    setLayout(m_layout);
}

QString SearchView::title()
{
    return tr("نافذة البحث");
}
