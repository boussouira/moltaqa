#ifndef INDEXWIDGET_H
#define INDEXWIDGET_H

#include "librarybook.h"
#include "bookindexmodel.h"
#include <qdockwidget.h>

namespace Ui {
    class IndexWidget;
}

class BookPage;
class QTreeView;

class IndexWidget : public QWidget
{
    Q_OBJECT
public:
    IndexWidget(QWidget *parent = 0);
    ~IndexWidget();

    void setBookInfo(LibraryBook *book);
    void setCurrentPage(BookPage *page);
    void hideAyaSpin(bool visible);
    QModelIndex selectTitle(int tid);

    QTreeView *treeView();
    BookIndexModel *indexModel();

    QModelIndex findTitle(int tid, bool checkSelected=false);

protected:
    QModelIndex fitchChild(QModelIndex parent, int tid);

public slots:
    void setIndex(BookIndexModel *indexModel);
    void displayBookInfo();
    void setSelectedSora(int pSoraNumber);

private slots:
    void listDoubleClicked(QModelIndex pIndex);
    void openPageInNewTab();

signals:
    void openSora(int sora, int aya);
    void openPage(int pageID);

    void openSoraInNewTab(int sora, int aya);
    void openPageInNewTab(int pageID);

    void bookInfoChanged();

private:
    BookIndexModel *m_model;
    LibraryBook *m_bookInfo;
    BookPage *m_page;
    Ui::IndexWidget *ui;
    bool sendSignals;
};

#endif // INDEXWIDGET_H
