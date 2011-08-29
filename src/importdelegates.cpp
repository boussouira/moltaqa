#include "importdelegates.h"
#include "importmodel.h"
#include "catslistwidget.h"
#include <qcombobox.h>

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

// Cat delegate
CategorieDelegate::CategorieDelegate(QObject* parent, QAbstractItemModel *model):
        QItemDelegate(parent), m_model(model)
{
}


QWidget *CategorieDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem &/*option*/,
                                       const QModelIndex &/*index*/) const
{
    CatsListWidget *browser = new CatsListWidget(parent);

    browser->setViewModel(m_model);
    connect(browser, SIGNAL(itemSelected()), this, SLOT(commitAndCloseEditor()));

    return browser;
}

void CategorieDelegate::setEditorData(QWidget *editor,
                                      const QModelIndex &index) const
{
    Q_UNUSED(editor)
    Q_UNUSED(index)

    /*
    CatsListWidget *browser = qobject_cast<CatsListWidget*>(editor);
    browser->selectIndex(index);
    */
}

void CategorieDelegate::setModelData(QWidget *editor,
                                     QAbstractItemModel *model,
                                     const QModelIndex &index) const
{

    CatsListWidget *browser = qobject_cast<CatsListWidget*>(editor);

    QString catName = browser->lastSelectedName();
    int catID = browser->lastSelectedID();
    if(!catName.isEmpty()) {
        ImportModel *mod = static_cast<ImportModel*>(model);
        mod->nodeFromIndex(index)->catID = catID;
        if(catID == 0)
            mod->nodeFromIndex(index)->catName = tr("-- غير محدد --");

        model->setData(index, catName, Qt::EditRole);
    }
}

void CategorieDelegate::updateEditorGeometry(QWidget *editor,
                                             const QStyleOptionViewItem &option,
                                             const QModelIndex &/* index */) const
{
    editor->resize(option.rect.width(), editor->parentWidget()->height());
}

void CategorieDelegate::commitAndCloseEditor()
{
    CatsListWidget *editor = qobject_cast<CatsListWidget *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
