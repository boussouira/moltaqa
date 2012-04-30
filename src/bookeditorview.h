#ifndef BOOKEDITORVIEW_H
#define BOOKEDITORVIEW_H

#include "abstarctview.h"
#include "librarybook.h"
#include "bookpage.h"
#include "bookeditor.h"

namespace Ui {
    class BookEditorView;
}

class EditWebView;
class RichBookReader;
class QTabWidget;
class BookIndexEditor;

class BookEditorView : public AbstarctView
{
    Q_OBJECT
public:
    BookEditorView(QWidget *parent = 0);
    ~BookEditorView();
    
    QString title();
    void editBook(LibraryBookPtr book, int pageID=0);
    bool maySave(bool canCancel=true);
    
protected:
    void setupView();
    void setupToolBar();
    void updateActions();
    bool pageEdited();
    void saveCurrentPage();
    void clearChanges();
    void closeBook(bool hide=true);

    void setCurrentPage(BookPage *page);

protected slots:
    void save();
    void cancel();
    void closeTab(int);

    void addPage();
    void removePage();

    void nextPage();
    void prevPage();
    void firstPage();
    void lastPage();
    void gotoPage();

    void readerTextChange();
    void checkPageModified();
    void indexChanged();

protected:
    friend class BookIndexEditor;

    Ui::BookEditorView *ui;
    QTabWidget *m_tabWidget;
    BookIndexEditor *m_indexEditor;
    QAction *m_actionSave;
    QAction *m_actionAddPage;
    QAction *m_actionRemovePage;
    QAction *m_actionNextPage;
    QAction *m_actionPrevPage;
    QAction *m_actionFirstPage;
    QAction *m_actionLastPage;
    QAction *m_actionGotToPage;
    EditWebView *m_webView;
    RichBookReader *m_bookReader;
    BookEditor *m_bookEditor;
    QHash<int, BookPage*> m_pages;
    BookPage *m_currentPage;
    QTimer *m_timer;
    bool m_indexEdited;
};

#endif // BOOKEDITORVIEW_H
