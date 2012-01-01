#ifndef BOOKEDITORVIEW_H
#define BOOKEDITORVIEW_H

#include "abstarctview.h"
#include "librarybook.h"
#include "bookpage.h"

namespace Ui {
    class BookEditorView;
}

class EditWebView;
class RichBookReader;
class QTabWidget;

class BookEditorView : public AbstarctView
{
    Q_OBJECT
public:
    BookEditorView(QWidget *parent = 0);
    ~BookEditorView();
    
    QString title();
    void editBook(LibraryBook *book, BookPage *page=0);
    bool maySave(bool canCancel=true);
    
protected:
    void setupView();
    void setupToolBar();
    void updateActions();
    void saveCurrentPage();
    void clearChanges();
    void closeBook();

protected slots:
    void save();
    void cancel();
    void preview();
    void closeTab(int);

    void nextPage();
    void prevPage();
    void firstPage();
    void lastPage();
    void gotoPage();

    void readerTextChange();

protected:
    Ui::BookEditorView *ui;
    QTabWidget *m_tabWidget;
    QAction *m_actionSave;
    QAction *m_actionCancel;
    QAction *m_actionPreview;
    QAction *m_actionNextPage;
    QAction *m_actionPrevPage;
    QAction *m_actionFirstPage;
    QAction *m_actionLastPage;
    QAction *m_actionGotToPage;
    EditWebView *m_webView;
    RichBookReader *m_bookReader;
    QHash<int, BookPage*> m_pages;
    BookPage *m_currentPage;
};

#endif // BOOKEDITORVIEW_H