#ifndef CONTROLCENTERDIALOG_H
#define CONTROLCENTERDIALOG_H

#include <qdialog.h>
#include "indexdb.h"
#include "editablebookslistmodel.h"

namespace Ui {
    class ControlCenterDialog;
}

class ControlCenterDialog : public QDialog
{
    Q_OBJECT

public:
    ControlCenterDialog(QWidget *parent = 0);
    ~ControlCenterDialog();

protected slots:
    void cutNode();
    void pastNode();
    void pastSublingNode();
    void moveUp();
    void moveDown();

protected slots:
    void menuRequested(QPoint);

private:
    IndexDB *m_indexDB;
    EditableBooksListModel *m_catsModel;
    BooksListNode *m_copiedNode;
    Ui::ControlCenterDialog *ui;
};

#endif // CONTROLCENTERDIALOG_H
