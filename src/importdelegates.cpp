#include "importdelegates.h"
#include "importmodel.h"
#include "selectcatdialog.h"
#include "selectauthordialog.h"
#include "shamelamanager.h"
#include "modelenums.h"
#include <qcombobox.h>

/* Book type delegate */
BookTypeDelegate::BookTypeDelegate(QObject* parent): QItemDelegate(parent)
{
}


QWidget *BookTypeDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/*option*/,
                                       const QModelIndex &/*index*/) const
{
    QComboBox *box = new QComboBox(parent);
    box->addItem(tr("مصحف"));
    box->addItem(tr("تفسير"));
    box->addItem(tr("عادي"));
    return box;

}

void BookTypeDelegate::setEditorData(QWidget *editor,
                   const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();

    QComboBox *box = static_cast<QComboBox*>(editor);
    for(int i=0; i<box->count(); i++){
        if(value == box->itemText(i))
            box->setCurrentIndex(i);
    }
}

void BookTypeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                  const QModelIndex &index) const
{
    QComboBox *box = static_cast<QComboBox*>(editor);
    QString value = box->currentText();

    model->setData(index, value, Qt::EditRole);
}

void BookTypeDelegate::updateEditorGeometry(QWidget *editor,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

/* Categorie delegate */
CategorieDelegate::CategorieDelegate(QObject* parent): QItemDelegate(parent)
{
}


QWidget *CategorieDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/*option*/,
                                       const QModelIndex &/*index*/) const
{
    selectCatDialog *browser = new selectCatDialog(parent);
    connect(browser, SIGNAL(catSelected()), SLOT(commitAndCloseEditor()));

    return browser;
}

void CategorieDelegate::setEditorData(QWidget *, const QModelIndex &) const
{
}

void CategorieDelegate::setModelData(QWidget *editor,
                                     QAbstractItemModel *model,
                                     const QModelIndex &index) const
{

    selectCatDialog *browser = qobject_cast<selectCatDialog*>(editor);

    if(browser->selectedNode()) {
        QString catName = browser->selectedCatName();
        int catID = browser->selectedCatID();

        ImportModel *importModel = qobject_cast<ImportModel*>(model);
        if(importModel)
            importModel->nodeFromIndex(index)->catID = catID;
        else
            model->setData(index, catID, ItemRole::idRole);

        model->setData(index, catName, Qt::EditRole);
    }
}

void CategorieDelegate::updateEditorGeometry(QWidget *,
                                             const QStyleOptionViewItem &,
                                             const QModelIndex &) const
{
}

void CategorieDelegate::commitAndCloseEditor()
{
    selectCatDialog *editor = qobject_cast<selectCatDialog *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

/* Choose Author delegate */
AuthorDelegate::AuthorDelegate(QObject* parent): QItemDelegate(parent)
{
}


QWidget *AuthorDelegate::createEditor(QWidget *parent,
                                         const QStyleOptionViewItem &/*option*/,
                                         const QModelIndex &/*index*/) const
{
    selectAuthorDialog *browser = new selectAuthorDialog(parent);
    connect(browser, SIGNAL(authorSelected()), SLOT(commitAndCloseEditor()));

    return browser;
}

void AuthorDelegate::setEditorData(QWidget *, const QModelIndex &) const
{
}

void AuthorDelegate::setModelData(QWidget *editor,
                                     QAbstractItemModel *model,
                                     const QModelIndex &index) const
{

    selectAuthorDialog *browser = qobject_cast<selectAuthorDialog*>(editor);

    if(browser->selectedAuthorID()) {
        QString authName = browser->selectedAuthorName();
        int authID = browser->selectedAuthorID();

        ImportModel *mod = static_cast<ImportModel*>(model);
        ImportModelNode *node = mod->nodeFromIndex(index);
        node->authorName = authName;
        node->authorID = authID;

        model->setData(index, authName, Qt::EditRole);
    }
}

void AuthorDelegate::updateEditorGeometry(QWidget *,
                                             const QStyleOptionViewItem &,
                                             const QModelIndex &) const
{
}

void AuthorDelegate::commitAndCloseEditor()
{
    selectAuthorDialog *editor = qobject_cast<selectAuthorDialog *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
