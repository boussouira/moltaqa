#ifndef EDITBOOKSLISTWIDGET_H
#define EDITBOOKSLISTWIDGET_H

#include "controlcenterwidget.h"
#include "librarybook.h"
#include <qhash.h>
#include <QModelIndex>

namespace Ui {
    class LibraryBookManagerWidget;
}

class LibraryManager;
class QStandardItemModel;
class LibraryBookManager;

class LibraryBookManagerWidget : public ControlCenterWidget
{
    Q_OBJECT

public:
    LibraryBookManagerWidget(QWidget *parent = 0);
    ~LibraryBookManagerWidget();

    QString title();

    void loadModel();
    void save();

protected:
    void enableEditWidgets(bool enable);
    void setupActions();
    void saveCurrentBookInfo();
    LibraryBookPtr getBookInfo(int bookID);

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_toolChangeAuthor_clicked();
    void setupEdit(LibraryBookPtr info);
    void infoChanged();

protected:
    Ui::LibraryBookManagerWidget *ui;
    LibraryBookPtr m_currentBook;
    QStandardItemModel *m_model;
    LibraryBookManager *m_manager;
    QHash<int, LibraryBookPtr > m_editedBookInfo;
};

#endif // EDITBOOKSLISTWIDGET_H
