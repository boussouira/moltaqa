#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <qsortfilterproxymodel.h>

class SortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    SortFilterProxyModel(QObject *parent = 0);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool filterAcceptsRowItself(int source_row, const QModelIndex &source_parent) const;
    bool hasAcceptedChildren(int source_row, const QModelIndex &source_parent) const;

    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

public slots:
    void setArabicFilterRegexp(QString text);

protected:
    bool m_showQuranFirst;
};

#endif // SORTFILTERPROXYMODEL_H
