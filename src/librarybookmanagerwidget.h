#ifndef EDITBOOKSLISTWIDGET_H
#define EDITBOOKSLISTWIDGET_H

#include "controlcenterwidget.h"
#include <qhash.h>
#include <QModelIndex>

namespace Ui {
    class LibraryBookManagerWidget;
}

class LibraryManager;
class LibraryBook;
class QStandardItemModel;
class LibraryBookManager;

class LibraryBookManagerWidget : public ControlCenterWidget
{
    Q_OBJECT

public:
    LibraryBookManagerWidget(QWidget *parent = 0);
    ~LibraryBookManagerWidget();

    QString title();
    void enableEditWidgets(bool enable);

protected:
    void loadModel();
    void setupActions();
    void saveCurrentBookInfo();
    LibraryBook *getBookInfo(int bookID);

public slots:
    void save();
    void beginEdit();

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_toolChangeAuthor_clicked();
    void setupEdit(LibraryBook *info);
    void infoChanged();

protected:
    Ui::LibraryBookManagerWidget *ui;
    LibraryBook *m_currentBook;
    QStandardItemModel *m_model;
    LibraryBookManager *m_manager;
    QHash<int, LibraryBook *> m_editedBookInfo;
};

#endif // EDITBOOKSLISTWIDGET_H
