#ifndef CATSLISTWIDGET_H
#define CATSLISTWIDGET_H

#include <qwidget.h>
#include <qabstractitemmodel.h>

class QTreeView;

class CatsListWidget : public QWidget
{
    Q_OBJECT

public:
    CatsListWidget(QWidget *parent = 0);
    void setViewModel(QAbstractItemModel *model);
    void selectIndex(const QModelIndex &index);
    QString lastSelectedName() { return m_lastSelected; }
    int lastSelectedID() { return m_lastSelectedID; }

protected slots:
    void itemDoubleClick(const QModelIndex & index);

signals:
    void itemSelected();

protected:
    QAbstractItemModel *m_model;
    QTreeView *m_treeView;
    QString m_lastSelected;
    int m_lastSelectedID;
};

#endif // CATSLISTWIDGET_H
