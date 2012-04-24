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
     void writeItem(QStandardItem *item, QXmlStreamWriter *writer);

signals:
     void indexEdited();

private:
    Ui::BookIndexEditor *ui;
    BookEditorView *m_editView;
    QStandardItemModel *m_model;
};

#endif // BOOKINDEXEDITOR_H
