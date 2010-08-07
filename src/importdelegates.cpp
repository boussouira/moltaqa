#include "importdelegates.h"
#include "importmodel.h"
#include "bookslistbrowser.h"
#include <qcombobox.h>

BookTypeDelegate::BookTypeDelegate(QObject* parent): QItemDelegate(parent)
{
}


QWidget *BookTypeDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/*option*/,
                                       const QModelIndex &/*index*/) const
{
    QComboBox *box = new QComboBox(parent);
    box->addItem(trUtf8("مصحف"));
    box->addItem(trUtf8("تفسير"));
    box->addItem(trUtf8("متن حديث"));
    box->addItem(trUtf8("عادي"));
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

// Cat delegate
CategorieDelegate::CategorieDelegate(QObject* parent): QItemDelegate(parent)
{
}


QWidget *CategorieDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/*option*/,
                                       const QModelIndex &/*index*/) const
{
    BooksListBrowser *browser = new BooksListBrowser(parent, false);
    browser->hideViewHeaders(true);
    connect(browser, SIGNAL(accepted()),
            this, SLOT(commitAndCloseEditor()));
    return browser;

}

void CategorieDelegate::setEditorData(QWidget */*editor*/,
                                      const QModelIndex &/*index*/) const
{
    /*
    QString value = index.model()->data(index, Qt::EditRole).toString();

    QComboBox *box = static_cast<QComboBox*>(editor);
    for(int i=0; i<box->count(); i++){
        if(value == box->itemText(i))
            box->setCurrentIndex(i);
    }*/
}

void CategorieDelegate::setModelData(QWidget *editor,
                                     QAbstractItemModel *model,
                                     const QModelIndex &index) const
{

    BooksListBrowser *browser = static_cast<BooksListBrowser*>(editor);
    QString catName = browser->lastSelectedName();
    int catID = browser->lastSelectedID();
    if(!catName.isEmpty()) {
        ImportModel *mod = static_cast<ImportModel*>(model);
        mod->nodeFromIndex(index)->setCatID(catID);

        model->setData(index, catName, Qt::EditRole);
    }
}

void CategorieDelegate::updateEditorGeometry(QWidget *editor,
                                             const QStyleOptionViewItem &option,
                                             const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect.x(), option.rect.y(),
                        400, 350);
}

void CategorieDelegate::commitAndCloseEditor()
{
    BooksListBrowser *editor = qobject_cast<BooksListBrowser *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
