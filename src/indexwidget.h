#ifndef INDEXWIDGET_H
#define INDEXWIDGET_H

#include "librarybook.h"
#include <qdockwidget.h>
#include <qstandarditemmodel.h>

namespace Ui {
    class IndexWidget;
}

class BookPage;
class QTreeView;
class ModelViewFilter;

class IndexWidget : public QWidget
{
    Q_OBJECT
public:
    IndexWidget(QWidget *parent = 0);
    ~IndexWidget();

    void setBookInfo(LibraryBookPtr book);
    void setCurrentPage(BookPage *page);
    void hideAyaSpin(bool visible);
    void selectTitle(int tid);

    QTreeView *treeView();
    QStandardItemModel *indexModel();

public slots:
    void setIndex(QStandardItemModel *indexModel);
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
    ModelViewFilter *m_filter;
    QStandardItemModel *m_model;
    LibraryBookPtr m_bookInfo;
    BookPage *m_page;
    Ui::IndexWidget *ui;
    bool sendSignals;
};

#endif // INDEXWIDGET_H
