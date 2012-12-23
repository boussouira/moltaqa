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
class EditWebView;
class ModelViewFilter;
class BookReaderView;

class LibraryBookManagerWidget : public ControlCenterWidget
{
    Q_OBJECT

public:
    LibraryBookManagerWidget(QWidget *parent = 0);
    ~LibraryBookManagerWidget();

    QString title();
    void aboutToShow();
    void aboutToHide();

    void loadModel();
    void save();

protected:
    void enableEditWidgets(bool enable);
    void setupActions();
    void setupBookReader();
    void saveCurrentBookInfo();
    LibraryBook::Ptr getBookInfo(int bookID);

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_tabWidget_currentChanged(int index);
    void on_toolChangeAuthor_clicked();
    void setupEdit(LibraryBook::Ptr info);
    void infoChanged();
    void checkEditWebChange();
    void createNewBook();
    void removeBook();
    void reindexBook();
    void lastReaderTabClosed();

protected:
    Ui::LibraryBookManagerWidget *ui;
    LibraryBook::Ptr m_currentBook;
    QStandardItemModel *m_model;
    LibraryBookManager *m_manager;
    EditWebView *m_webEdit;
    BookReaderView *m_readerview;
    QWidget *m_readerWidget;
    ModelViewFilter *m_filter;
    QHash<int, LibraryBook::Ptr > m_editedBookInfo;
    bool m_selectCurrentBook;
};

#endif // EDITBOOKSLISTWIDGET_H
