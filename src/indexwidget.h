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

    void hideAyaSpin(bool visible);
    void hidePageSpin(bool visible);
    void hidePartSpin(bool visible);
    void setBookInfo(LibraryBook *bInfo) { m_bookInfo = bInfo; }
    void setCurrentPage(BookPage *page) { m_page = page; }
    void selectTitle(int tid);

    QTreeView *treeView();

protected:
    void changeEvent(QEvent *e);
    bool fitchChild(QModelIndex parent, int tid);

public slots:
    void setIndex(BookIndexModel *indexModel);
    void displayBookInfo();
    void setSelectedSora(int pSoraNumber);
    void updatePageAndAyaNum(int pPageNumber, int pAyaNumber);
    int currentPageNmber();
    void updateAyaNumber(int pAyaNumber);

private slots:
    void ayaNumChange(int pAyaNum);
    void listDoubleClicked(QModelIndex pIndex);
    void openSoraInCurrentTab();
    void openSoraInNewTab();

signals:
    void ayaNumberChange(int pNewAyaNumber);
    void openSora(int sora, int aya);
    void openSoraInNewTab(int pSoraNumber);
    void openPage(int pageID);
    void bookInfoChanged();

private:
    BookIndexModel *m_model;
    LibraryBook *m_bookInfo;
    BookPage *m_page;
    Ui::IndexWidget *ui;
    bool sendSignals;
};

#endif // INDEXWIDGET_H
