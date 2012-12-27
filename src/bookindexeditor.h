#ifndef BOOKINDEXEDITOR_H
#define BOOKINDEXEDITOR_H

#include <qwidget.h>
#include <qstandarditemmodel.h>

namespace Ui {
class BookIndexEditor;
}

class QXmlStreamWriter;
class EditWebView;
class RichBookReader;
class BookEditorView;
class TreeViewEditor;

class BookIndexEditor : public QWidget
{
    Q_OBJECT
    
public:
     BookIndexEditor(BookEditorView *parent);
    ~BookIndexEditor();

     void setup();
     void saveModel(QXmlStreamWriter *writer);
     QString save();

protected slots:
     void addTitle();
     void linkTitle();
     void updateActions();
     void openPage(QModelIndex index);

protected:
     void setModel(QStandardItemModel *model);
     void writeItem(QStandardItem *item, QXmlStreamWriter *writer);

signals:
     void indexEdited();

private:
    Ui::BookIndexEditor *ui;
    BookEditorView *m_editView;
    QStandardItemModel *m_model;
    TreeViewEditor *m_treeManager;
};

#endif // BOOKINDEXEDITOR_H
