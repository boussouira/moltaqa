#ifndef BOOKREADERVIEW_H
#define BOOKREADERVIEW_H

#include "abstarctview.h"
#include "clutils.h"
#include "librarybook.h"

class BookViewBase;
class LibraryManager;
class LibraryBookManager;
class TaffesirListManager;
class TabWidget;
class QMainWindow;
class QToolBar;
class QMenu;
class QComboBox;
class CLuceneQuery;
class FilterLineEdit;
class BookWidgetManager;

namespace Phonon {
class MediaObject;
class AudioOutput;
class MediaSource;
}

class BookReaderView : public AbstarctView
{
    Q_OBJECT
public:
    BookReaderView(LibraryManager *libraryManager, QWidget *parent=0);
    ~BookReaderView();

    void setLibraryManager(LibraryManager *libraryManager) { m_libraryManager = libraryManager;}
    QString title();
    QString viewLink();
    WebViewSearcher *searcher();
    void updateToolBars();

    int currentBookID();
    BookViewBase *currentBookWidget();
    LibraryBook::Ptr currentBook();
    BookPage *currentPage();

    BookWidgetManager *bookWidgetManager();

public slots:
    BookViewBase *openBook(int bookID, int pageID = -1, CLuceneQuery *query=0);
    void updateActions();
    void toggleIndexWidget();
    void searchInBook();
    void openTafessir();
    void tabChanged(int newIndex);
    void loadTafessirList();
    void removeTashkil(bool remove);


    // TODO: Recode these methods properly (remove code dupliction)
    void getSheer();
    void getMateen();

protected slots:
    void editCurrentBook();

    void quranPlayToggle();
    void playerAboutToFinnish();
    void playerFinnish();
    void playerSourceChanged(const Phonon::MediaSource &);

protected:
    void createMenus();
    void playCurentAya();

signals:
    void lastTabClosed();

private:
    LibraryManager *m_libraryManager;
    LibraryBookManager *m_bookManager;
    TaffesirListManager *m_taffesirManager;
    BookWidgetManager *m_viewManager;
    QAction *m_actionEditBook;
    QAction *m_actionNewTab;
    QAction *m_actionIndexDock;
    QAction *m_actionSearchInBook;
    QAction *m_actionNextAYA;
    QAction *m_actionNextPage;
    QAction *m_actionPrevAYA;
    QAction *m_actionPrevPage;
    QAction *m_actionFirstPage;
    QAction *m_actionLastPage;
    QAction *m_actionGotToPage;
    QAction *m_bookInfoAct;
    QAction *m_removeTashekilAct;
    QAction *m_openSelectedTafsir;
    QAction *m_playQuranAudio;
    QToolBar *m_toolBarGeneral;
    QToolBar *m_toolBarNavigation;
    QToolBar *m_toolBarTafesir;
    QComboBox *m_comboTafasir;
    QComboBox *m_comboQuranReciter;

    Phonon::MediaObject *m_mediaObject;
    Phonon::AudioOutput *m_audioOutput;
    bool m_handleSourceChange;
};

#endif // BOOKREADERVIEW_H
