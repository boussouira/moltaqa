#ifndef BOOKEDITORVIEW_H
#define BOOKEDITORVIEW_H

#include "abstarctview.h"
#include "librarybook.h"
#include "bookpage.h"

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
    
protected:
    void setupToolBar();
    void updateActions();
    void saveCurrentPage();
    void clearChanges();
    bool maySave();
    void closeBook();

protected slots:
    void save();
    void cancel();
    void preview();
    void closeTab(int index);

    void nextPage();
    void prevPage();
    void firstPage();
    void lastPage();
    void gotoPage();

    void readerTextChange();

protected:
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
