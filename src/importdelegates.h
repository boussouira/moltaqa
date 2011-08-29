#ifndef IMPORTDELEGATES_H
#define IMPORTDELEGATES_H

#include <qitemdelegate.h>

class BookTypeDelegate: public QItemDelegate
{
public:
    BookTypeDelegate(QObject* parent = 0);


    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &/* index */) const;
};

class CategorieDelegate: public QItemDelegate
{
    Q_OBJECT
public:
    CategorieDelegate(QObject* parent = 0);


    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &/* index */) const;
protected slots:
    void commitAndCloseEditor();
};

class AuthorDelegate: public QItemDelegate
{
    Q_OBJECT
public:
    AuthorDelegate(QObject* parent = 0);


    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &/* index */) const;
protected slots:
    void commitAndCloseEditor();
};

#endif // IMPORTDELEGATES_H
