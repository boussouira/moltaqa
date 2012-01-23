#ifndef BOOKINDEXEDITOR_H
#define BOOKINDEXEDITOR_H

#include <qwidget.h>
#include <qxmlstream.h>

namespace Ui {
class BookIndexEditor;
}

class BookIndexModel;
class BookIndexNode;

class BookIndexEditor : public QWidget
{
    Q_OBJECT
    
public:
     BookIndexEditor(QWidget *parent = 0);
    ~BookIndexEditor();
    
     void setModel(BookIndexModel *model);
     void saveModel(QXmlStreamWriter *writer);

     bool save(QString path);

protected:
     void writeNode(BookIndexNode *node, QXmlStreamWriter *writer);

private:
    Ui::BookIndexEditor *ui;
    BookIndexModel *m_model;
};

#endif // BOOKINDEXEDITOR_H
