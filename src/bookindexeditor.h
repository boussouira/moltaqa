#ifndef BOOKINDEXEDITOR_H
#define BOOKINDEXEDITOR_H

#include <qwidget.h>
#include <qxmlstream.h>

namespace Ui {
class BookIndexEditor;
}

class QStandardItemModel;
class QStandardItem;

class BookIndexEditor : public QWidget
{
    Q_OBJECT
    
public:
     BookIndexEditor(QWidget *parent = 0);
    ~BookIndexEditor();
    
     void setModel(QStandardItemModel *model);
     void saveModel(QXmlStreamWriter *writer);

     bool save(QString path);

protected:
     void writeItem(QStandardItem *item, QXmlStreamWriter *writer);

private:
    Ui::BookIndexEditor *ui;
    QStandardItemModel *m_model;
};

#endif // BOOKINDEXEDITOR_H
