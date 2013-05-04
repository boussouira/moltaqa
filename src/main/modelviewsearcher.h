#ifndef MODELVIEWSEARCHER_H
#define MODELVIEWSEARCHER_H

#include <qobject.h>

class FilterLineEdit;
class QTreeView;
class QStandardItemModel;
class QStandardItem;

class ModelViewSearcher : public QObject
{
     Q_OBJECT

public:
    ModelViewSearcher(QObject *parent = 0);

    void setSourceModel(QStandardItemModel *model);
    void setLineEdit(FilterLineEdit *edit);
    void setTreeView(QTreeView *view);

    void setup();

protected:
    bool search(QStandardItem *item, QString text);

public slots:
    void clearFilter();

protected slots:
    void setFilterText(QString text);
    void lineReturnPressed();

protected:
    QTreeView *m_treeView;
    QStandardItemModel *m_model;
    FilterLineEdit *m_lineEdit;
    QList<QStandardItem*> m_skipItems;
    bool m_loopSearch;
};

#endif // MODELVIEWSEARCHER_H
