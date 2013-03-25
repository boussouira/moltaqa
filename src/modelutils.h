#ifndef MODELUTILS_H
#define MODELUTILS_H

#include <qstandarditemmodel.h>

class QTreeView;
class QAbstractButton;

namespace Utils {
namespace Model {

QModelIndex findModelIndex(QAbstractItemModel *model, int tid);

QStandardItem *itemFromIndex(QStandardItemModel *model, const QModelIndex &index);

QModelIndex changeParent(QStandardItemModel *model, QModelIndex child, QModelIndex newParent, int row=-1);
void swap(QStandardItemModel *model, QModelIndex fromIndex, QModelIndex toIndex);

QModelIndex selectedIndex(QTreeView *tree);
void selectIndex(QTreeView *tree, const QModelIndex &index);

bool indexesAtSameLevel(QList<QModelIndex> &list);

void moveUp(QStandardItemModel *model, QTreeView *tree);
void moveDown(QStandardItemModel *model, QTreeView *tree);
void moveRight(QStandardItemModel *model, QTreeView *tree);
void moveLeft(QStandardItemModel *model, QTreeView *tree);

QStandardItemModel *cloneModel(QStandardItemModel *model);
QList<QStandardItem *> cloneItem(QStandardItem *item, QStandardItem *parent, int columnCount);
void setModelCheckable(QStandardItemModel *model, bool checkable=true);

}
}

class TreeViewEditor : public QObject
{
    Q_OBJECT
public:
    TreeViewEditor(QObject *parent=0);
    ~TreeViewEditor();

    void setTreeView(QTreeView *tree);
    void setModel(QStandardItemModel *model);
    void setup();

    void setMoveUpButton(QAbstractButton *btn);
    void setMoveDownButton(QAbstractButton *btn);
    void setMoveLeftButton(QAbstractButton *btn);
    void setMoveRightButton(QAbstractButton *btn);
    void setRemovButton(QAbstractButton *btn);

    bool isDataChanged() { return m_dataChanged; }
    void setDataChanged(bool changed) { m_dataChanged = changed; }

public slots:
    void dataChanged();
    void updateActions();

    void moveUp();
    void moveDown();
    void moveRight();
    void moveLeft();
    void removeItem();

signals:
    void modelDataChanged();

protected:
    QTreeView *m_tree;
    QStandardItemModel *m_model;
    QAbstractButton *m_moveUp;
    QAbstractButton *m_moveDown;
    QAbstractButton *m_moveLeft;
    QAbstractButton *m_moveRight;
    QAbstractButton *m_remove;
    QModelIndex m_lastLeftParent;
    int m_lastLeftRow;
    bool m_dataChanged;
};

#endif // MODELUTILS_H
