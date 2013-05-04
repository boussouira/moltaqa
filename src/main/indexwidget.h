#ifndef INDEXWIDGET_H
#define INDEXWIDGET_H

#include "librarybook.h"

#include <qabstractitemmodel.h>
#include <qwidget.h>

namespace Ui {
    class IndexWidget;
}

class BookPage;
class ModelViewFilter;
class QTreeView;
class QStandardItemModel;

class IndexWidget : public QWidget
{
    Q_OBJECT
public:
    IndexWidget(QWidget *parent = 0);
    ~IndexWidget();

    void setBookInfo(LibraryBook::Ptr book);
    void setCurrentPage(BookPage *page);
    void hideAyaSpin(bool visible);
    void selectTitle(int tid);

    QTreeView *treeView();
    QStandardItemModel *indexModel();

public slots:
    void setIndex(QStandardItemModel *indexModel);
    void displayBookInfo();
    void setSelectedSora(int pSoraNumber);
    void updateCurrentTitle(bool checked);

private slots:
    void listClicked(QModelIndex index);
    void scrollToTitle();

signals:
    void openSora(int sora, int aya);
    void openPage(int pageID);
    void scrollToElement(QString selector, bool center);

    void bookInfoChanged();

private:
    ModelViewFilter *m_filter;
    QStandardItemModel *m_model;
    LibraryBook::Ptr m_bookInfo;
    BookPage *m_page;
    Ui::IndexWidget *ui;
    bool sendSignals;
    QString m_tid;
};

#endif // INDEXWIDGET_H
