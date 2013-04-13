#include "windowsview.h"
#include "abstarctview.h"
#include "modelutils.h"

#include <qboxlayout.h>
#include <qlistwidget.h>
#include <qlabel.h>

WindowsView::WindowsView(QWidget *parent) :
    QDialog(parent)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setWindowTitle(tr("مكتبة الملتقى"));

    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    verticalLayout->setMargin(0);

    m_listWidget = new QListWidget(this);

    verticalLayout->addWidget(m_listWidget);

    resize(300, 200);
}

WindowsView::~WindowsView()
{
}

void WindowsView::addView(AbstarctView *view, int index)
{
    QListWidgetItem *item = new QListWidgetItem(view->title(), m_listWidget);
    item->setData(Qt::UserRole, index);

    m_listWidget->addItem(item);
}

void WindowsView::selectCurrentView()
{
    QModelIndex index = m_listWidget->model()->index(0, 0);
    if (index.isValid()) {
        m_listWidget->setCurrentIndex(index);
        m_listWidget->scrollTo(index, QAbstractItemView::PositionAtCenter);
    }

    connect(m_listWidget->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(currentViewChanged(QModelIndex,QModelIndex)));

    connect(m_listWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            SLOT(itemClicked(QListWidgetItem*)));
}

void WindowsView::selectNextItem(int summand)
{
    const int pageCount = m_listWidget->model()->rowCount();
    if (pageCount < 2)
        return;

    const QModelIndexList &list = m_listWidget->selectionModel()->selectedIndexes();
    if (list.isEmpty())
        return;

    QModelIndex index = list.first();
    if (!index.isValid())
        return;

    index = m_listWidget->model()->index((index.row() + summand + pageCount) % pageCount, 0);
    if (index.isValid()) {
        m_listWidget->setCurrentIndex(index);
        m_listWidget->scrollTo(index, QAbstractItemView::PositionAtCenter);
    }
}

void WindowsView::keyPressEvent(QKeyEvent *event)
{
    if((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_Tab) {
        selectNextItem(1);
    }
}

void WindowsView::keyReleaseEvent(QKeyEvent *event)
{
    if(!(event->modifiers() & Qt::ControlModifier)) {
        accept();
    }
}

void WindowsView::currentViewChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    if(current.isValid()) {
        emit selectView(current.data().toString());
    }
}

void WindowsView::itemClicked(QListWidgetItem *item)
{
    if(item) {
        selectView(item->text());
        accept();
    }
}
