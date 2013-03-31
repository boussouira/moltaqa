#ifndef BOOKINDEXEDITOR_H
#define BOOKINDEXEDITOR_H

#include <qwidget.h>
#include <qabstractitemmodel.h>

namespace Ui {
class BookIndexEditor;
}

class QXmlStreamWriter;
class EditWebView;
class RichBookReader;
class BookEditorView;
class TreeViewEditor;
class ZipHelper;
class QStandardItemModel;
class QStandardItem;

class BookIndexEditor : public QWidget
{
    Q_OBJECT
    
public:
     BookIndexEditor(BookEditorView *parent);
    ~BookIndexEditor();

     void setup();
     void saveModel(QXmlStreamWriter *writer);
     bool save(ZipHelper *zipHelper);

     bool indexEdited() const;
     void setIndexEdited(bool edited);

protected slots:
     void addTitle();
     void linkTitle();
     void updateActions();
     void openPage(QModelIndex index);
     void indexEditedSlot();

protected:
     void setModel(QStandardItemModel *model);
     void writeItem(QStandardItem *item, QXmlStreamWriter *writer);

signals:
     void indexDataChanged();

private:
    Ui::BookIndexEditor *ui;
    BookEditorView *m_editView;
    QStandardItemModel *m_model;
    TreeViewEditor *m_treeManager;
    bool m_indexEdited;
};

#endif // BOOKINDEXEDITOR_H
