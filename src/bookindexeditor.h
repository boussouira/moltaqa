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

class BookIndexEditor : public QWidget
{
    Q_OBJECT
    
public:
     BookIndexEditor(BookEditorView *parent);
    ~BookIndexEditor();

     void setup();
     void saveModel(QXmlStreamWriter *writer);
     bool save(QString path);

protected slots:
     void addTitle();
     void removeTitle();
     void moveUp();
     void moveDown();
     void moveRight();
     void moveLeft();
     void linkTitle();
     void updateActions();
     void openPage(QModelIndex index);

protected:
     void setModel(QStandardItemModel *model);

     QModelIndex selectedIndex();
     QModelIndex changeParent(QModelIndex child, QModelIndex newParent, int row=-1);
     void swap(QModelIndex fromIndex, QModelIndex toIndex);
     void selectIndex(QModelIndex index);
     void writeItem(QStandardItem *item, QXmlStreamWriter *writer);

private:
    Ui::BookIndexEditor *ui;
    BookEditorView *m_editView;
    QStandardItemModel *m_model;
};

#endif // BOOKINDEXEDITOR_H
