#include "sortfilterproxymodel.h"
#include "stringutils.h"
#include "modelenums.h"
#include "librarybook.h"
#include "utils.h"

SortFilterProxyModel::SortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    m_showQuranFirst = Utils::Settings::get("Style/showQuranFirst", true).toBool();
    m_filterByDeath = false;
    m_filterFromYear = -1;
    m_filterToYear = -1;
}

bool SortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (filterAcceptsRowItself(source_row, source_parent))
        return true;

    //accept if any of the parents is accepted on it's own merits
    QModelIndex parent = source_parent;
    while (parent.isValid()) {
        if (filterAcceptsRowItself(parent.row(), parent.parent()))
            return true;
        parent = parent.parent();
    }

    //accept if any of the children is accepted on it's own merits
    if (hasAcceptedChildren(source_row, source_parent)) {
        return true;
    }

    return false;
}

bool SortFilterProxyModel::filterAcceptsRowItself(int source_row, const QModelIndex &source_parent) const
{
    if(m_filterByDeath) {
        QModelIndex index = sourceModel()->index(source_row, 1, source_parent);
        bool ok;
        int deathYear = index.data(ItemRole::authorDeathRole).toInt(&ok);

        return (ok && m_filterFromYear <= deathYear && deathYear <= m_filterToYear);
    }

    if (filterRegExp().isEmpty())
        return true;
    if (filterKeyColumn() == -1) {
        int column_count = sourceModel()->columnCount(source_parent);
        for (int column = 0; column < column_count; ++column) {
            QModelIndex source_index = sourceModel()->index(source_row, column, source_parent);
            QString key = sourceModel()->data(source_index, filterRole()).toString();
            key = Utils::String::Arabic::clean(key);
            if (key.contains(filterRegExp()))
                return true;
        }
        return false;
    }

    QModelIndex source_index = sourceModel()->index(source_row, filterKeyColumn(), source_parent);
    if (!source_index.isValid()) // the column may not exist
        return true;
    QString key = sourceModel()->data(source_index, filterRole()).toString();
    key = Utils::String::Arabic::clean(key);

    return key.contains(filterRegExp());
}

bool SortFilterProxyModel::hasAcceptedChildren(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex item = sourceModel()->index(source_row,0,source_parent);
    if (!item.isValid()) {
        //qDebug() << "item invalid" << source_parent << source_row;
        return false;
    }

    //check if there are children
    int childCount = item.model()->rowCount(item);
    if (childCount == 0)
        return false;

    for (int i = 0; i < childCount; ++i) {
        if (filterAcceptsRowItself(i, item))
            return true;
        //recursive call
        if (hasAcceptedChildren(i, item))
            return true;
    }

    return false;
}

bool SortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if(m_showQuranFirst) {
        int leftType = left.model() ? left.model()->data(left, ItemRole::typeRole).toInt() : 0;
        int rightType = right.model() ? right.model()->data(right, ItemRole::typeRole).toInt() : 0;

        if(leftType == LibraryBook::QuranBook
                || rightType == LibraryBook::QuranBook) {
            if(leftType == LibraryBook::QuranBook) {
                return sortOrder()==Qt::AscendingOrder ? true : false;
            } else if(rightType == LibraryBook::QuranBook) {
                return sortOrder()==Qt::AscendingOrder ? false : true;
            }
        }
    }

    return QSortFilterProxyModel::lessThan(left, right);
}

void SortFilterProxyModel::setFilterByDeath(int fromYear, int toYear)
{
    m_filterByDeath = true;
    m_filterFromYear = fromYear;
    m_filterToYear = toYear;
    setFilterRegExp(QString("%1:%2").arg(fromYear).arg(toYear));
}

void SortFilterProxyModel::setArabicFilterRegexp(QString text)
{
    m_filterByDeath = false;
    setFilterRegExp(Utils::String::Arabic::clean(text));
}
