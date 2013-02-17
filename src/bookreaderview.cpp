#include "bookreaderview.h"
#include "tabwidget.h"
#include "indexwidget.h"
#include "bookslistbrowser.h"
#include "richsimplebookreader.h"
#include "richquranreader.h"
#include "richtafessirreader.h"
#include "librarymanager.h"
#include "bookwidget.h"
#include "bookexception.h"
#include "openpagedialog.h"
#include "modelenums.h"
#include "mainwindow.h"
#include "bookwidgetmanager.h"
#include "utils.h"
#include "bookeditorview.h"
#include "taffesirlistmanager.h"
#include "librarybookmanager.h"
#include "filterlineedit.h"
#include "webview.h"
#include "libraryinfo.h"

#include <qmainwindow.h>
#include <qmenubar.h>
#include <qmenu.h>
#include <qaction.h>
#include <qtoolbar.h>
#include <qcombobox.h>
#include <qstackedwidget.h>
#include <qboxlayout.h>
#include <qdebug.h>
#include <qmessagebox.h>
#include <qkeysequence.h>
#include <QCompleter>
#include <qprogressdialog.h>
#include <qfile.h>

BookReaderView::BookReaderView(LibraryManager *libraryManager, QWidget *parent): AbstarctView(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_viewManager = new BookWidgetManager(this);

    m_libraryManager = libraryManager;
    m_bookManager = m_libraryManager->bookManager();
    m_taffesirManager = m_libraryManager->taffesirListManager();

    layout->addWidget(m_viewManager);
    layout->setContentsMargins(0,6,0,0);

    createMenus();
    loadTafessirList();

    connect(m_viewManager, SIGNAL(currentTabChanged(int)), SLOT(updateActions()));
    connect(m_viewManager, SIGNAL(currentTabChanged(int)), SLOT(tabChanged(int)));
    connect(m_viewManager, SIGNAL(lastTabClosed()), SIGNAL(lastTabClosed()));
    connect(m_viewManager, SIGNAL(lastTabClosed()), SIGNAL(hideMe()));
}

BookReaderView::~BookReaderView()
{
    delete m_viewManager;
}

QString BookReaderView::title()
{
    return tr("تصفح الكتب");
}

void BookReaderView::createMenus()
{

    // Edit book action
    m_actionEditBook = new QAction(QIcon::fromTheme("document-edit", QIcon(":/images/document-edit.png")),
                                 tr("تحرير الكتاب"), this);
    // General Actions
    m_actionNewTab = new QAction(QIcon::fromTheme("tab-new", QIcon(":/images/tab-new.png")),
                                        tr("تبويب جديد"),
                                        this);

    m_actionIndexDock = new QAction(QIcon(":/images/edit_fahrass.png"),
                                           tr("نافذة الفهرس"),
                                           this);

    m_actionSearchInBook = new QAction(QIcon::fromTheme("edit-find", QIcon(":/images/find.png")),
                                            tr("بحث متقدم في هذا الكتاب"),
                                            this);

    // Navigation actions
    m_actionNextAYA = new QAction(QIcon::fromTheme("go-down", QIcon(":/images/go-down.png")),
                                tr("الآية التالية"),
                                this);
    m_actionNextPage = new QAction(QIcon::fromTheme("go-previous", QIcon(":/images/go-previous.png")),
                                 tr("الصفحة التالية"),
                                 this);
    m_actionPrevAYA = new QAction(QIcon::fromTheme("go-up", QIcon(":/images/go-up.png")),
                                tr("الآية السابقة"),
                                this);
    m_actionPrevPage = new QAction(QIcon::fromTheme("go-next", QIcon(":/images/go-next.png")),
                                 tr("الصفحة السابقة"),
                                 this);
    m_actionFirstPage = new QAction(QIcon::fromTheme("go-last", QIcon(":/images/go-last.png")),
                                tr("الصفحة الاولى"),
                                this);
    m_actionLastPage = new QAction(QIcon::fromTheme("go-first", QIcon(":/images/go-first.png")),
                                 tr("الصفحة الاخيرة"),
                                 this);
    m_actionGotToPage = new QAction(QIcon::fromTheme("go-jump", QIcon(":/images/go-jump.png")),
                                    tr("انتقل الى..."),
                                    this);

    m_bookInfoAct = new QAction(tr("بطاقة الكتاب"), this);
    QAction *readHistoryAct = new QAction(tr("تاريخ تصفح الكتاب"), this);

    m_removeTashekilAct = new QAction(tr("حذف التشكيل"), this);
    m_removeTashekilAct->setCheckable(true);
    m_removeTashekilAct->setChecked(Utils::Settings::get("Style/removeTashekil", false).toBool());

    QAction *getSheerAct = new QAction(tr("الأبيات الشعرية"), this);

    m_actionNextAYA->setShortcut(QKeySequence("J"));
    m_actionPrevAYA->setShortcut(QKeySequence("K"));
    m_actionNextPage->setShortcut(QKeySequence("N"));
    m_actionPrevPage->setShortcut(QKeySequence("P"));
    m_actionGotToPage->setShortcut(QKeySequence("G"));

    // Tafressir actions
    m_openSelectedTafsir =  new QAction(QIcon(":/images/arrow-left.png"),
                                               tr("فتح تفسير الاية"),
                                               this);
    m_comboTafasir = new QComboBox(this);

    // Add action to their toolbars
    m_toolBarGeneral = new QToolBar(tr("عام"), this);
    m_toolBarGeneral->addAction(m_actionNewTab);
    m_toolBarGeneral->addSeparator();
    m_toolBarGeneral->addAction(m_actionIndexDock);
    m_toolBarGeneral->addAction(m_actionSearchInBook);

    m_toolBarNavigation = new QToolBar(tr("التصفح"), this);
    m_toolBarNavigation->addAction(m_actionPrevPage);
    m_toolBarNavigation->addAction(m_actionNextPage);
    m_toolBarNavigation->addAction(m_actionNextAYA);
    m_toolBarNavigation->addAction(m_actionPrevAYA);
    m_toolBarNavigation->addAction(m_actionGotToPage);

    m_toolBarTafesir = new QToolBar(tr("التفاسير"), this);
    m_toolBarTafesir->addWidget(m_comboTafasir);
    m_toolBarTafesir->addAction(m_openSelectedTafsir);

    m_toolBarGeneral->setObjectName("BookReaderView.General");
    m_toolBarNavigation->setObjectName("BookReaderView.Navigation");
    m_toolBarTafesir->setObjectName("BookReaderView.Tafesir");
    m_toolBarSearch->setObjectName("BookReaderView.Search");

    updateSearchNavigation();

    m_navActions << m_actionEditBook;
    m_navActions << actionSeparator(this);
    m_navActions << m_actionFirstPage;
    m_navActions << m_actionPrevPage;
    m_navActions << m_actionNextPage;
    m_navActions << m_actionLastPage;
    m_navActions << m_actionGotToPage;
    m_navActions << actionSeparator(this);
    m_navActions << m_removeTashekilAct;
    m_navActions << actionSeparator(this);
    m_navActions << m_bookInfoAct;
    m_navActions << readHistoryAct;
    m_navActions << actionSeparator(this);
    m_navActions << getSheerAct;

    m_toolBars << m_toolBarGeneral;
    m_toolBars << m_toolBarNavigation;
    m_toolBars << m_toolBarTafesir;
    m_toolBars << m_toolBarSearch;

    connect(m_viewManager, SIGNAL(pageChanged()), SLOT(updateActions()));
    connect(m_actionEditBook, SIGNAL(triggered()), SLOT(editCurrentBook()));

    // Navigation actions
    connect(m_actionNextPage, SIGNAL(triggered()), m_viewManager, SLOT(nextPage()));
    connect(m_actionPrevPage, SIGNAL(triggered()), m_viewManager, SLOT(previousPage()));
    connect(m_actionNextAYA, SIGNAL(triggered()), m_viewManager, SLOT(nextAya()));
    connect(m_actionPrevAYA, SIGNAL(triggered()), m_viewManager, SLOT(previousAya()));
    connect(m_actionFirstPage, SIGNAL(triggered()), m_viewManager, SLOT(firstPage()));
    connect(m_actionLastPage, SIGNAL(triggered()), m_viewManager, SLOT(lastPage()));
    connect(m_actionGotToPage, SIGNAL(triggered()), m_viewManager, SLOT(goToPage()));
    connect(m_bookInfoAct, SIGNAL(triggered()), m_viewManager, SLOT(showBookInfo()));
    connect(readHistoryAct, SIGNAL(triggered()), m_viewManager, SLOT(showBookHistory()));
    connect(getSheerAct, SIGNAL(triggered()), SLOT(getSheer()));
    connect(m_removeTashekilAct, SIGNAL(triggered(bool)), SLOT(removeTashkil(bool)));

    // Generale actions
    connect(m_actionIndexDock, SIGNAL(triggered()), SLOT(showIndexWidget()));
    connect(m_actionNewTab, SIGNAL(triggered()), MW, SLOT(showBooksList()));
    connect(m_actionSearchInBook, SIGNAL(triggered()), this, SLOT(searchInBook()));

    // Tafessir actions
    connect(m_openSelectedTafsir, SIGNAL(triggered()), SLOT(openTafessir()));
    connect(m_taffesirManager, SIGNAL(ModelsReady()), SLOT(loadTafessirList()));
}

void BookReaderView::updateToolBars()
{
    LibraryBook::Ptr book = m_viewManager->activeBook();

    if(book) {
        bool showTafsssir = book->isQuran() && m_comboTafasir->count();

        m_toolBarTafesir->setEnabled(showTafsssir);
        m_toolBarTafesir->setVisible(showTafsssir);

        updateSearchNavigation();
    }
}

QString BookReaderView::viewLink()
{
    AbstractBookReader *bookReader = m_viewManager->activeBookReader();
    ml_return_val_on_fail(bookReader, QString());

    QString link = QString("moltaqa://open/");
    if(bookReader->book()->isQuran())
        link.append(QString("quran?sora=%1&aya=%2").arg(bookReader->page()->sora).arg(bookReader->page()->aya));
    else
        link.append(QString("book?id=%1&page=%2").arg(bookReader->book()->id).arg(bookReader->page()->pageID));

    return link;
}

WebViewSearcher *BookReaderView::searcher()
{
    ml_return_val_on_fail(currentBookWidget(), 0);
    return currentBookWidget()->viewSearcher();
}

int BookReaderView::currentBookID()
{
    LibraryBook::Ptr book = m_viewManager->activeBook();

    return book ? book->id : 0;
}

BookViewBase *BookReaderView::currentBookWidget()
{
    return m_viewManager->activeBookWidget();
}

LibraryBook::Ptr BookReaderView::currentBook()
{
    return m_viewManager->activeBook();
}

BookPage *BookReaderView::currentPage()
{
    AbstractBookReader *bookdb = m_viewManager->activeBookReader();

   return bookdb ? bookdb->page() : 0;
}

BookViewBase *BookReaderView::openBook(int bookID, int pageID, CLuceneQuery *query)
{
    try {
        LibraryBook::Ptr bookInfo = m_bookManager->getLibraryBook(bookID);
        if(!bookInfo)
            throw BookException(tr("لم يتم العثور على الكتاب المطلوب"), tr("معرف الكتاب: %1").arg(bookID));

        if(!bookInfo->exists())
            throw BookException(tr("لم يتم العثور على ملف"), bookInfo->path);

        BookViewBase *bookWidget = new BookWidget(bookInfo, this);
        m_viewManager->addBook(bookWidget);

        if(query && pageID != -1)
            bookWidget->bookReader()->highlightPage(pageID, query);

        if(pageID == -1)
            bookWidget->openPage(m_bookManager->bookLastPageID(bookID));
        else
            bookWidget->openPage(pageID);

        connect(bookWidget, SIGNAL(textChanged()), SLOT(updateActions()));

        updateActions();

        emit showMe();

        return bookWidget;

    } catch (BookException &e) {
        QMessageBox::critical(this,
                              tr("فتح كتاب"),
                              e.what());
        return 0;
    }
}

void BookReaderView::openTafessir()
{
    try {
        int tafessirID = m_comboTafasir->itemData(m_comboTafasir->currentIndex(), ItemRole::idRole).toInt();

        LibraryBook::Ptr bookInfo = m_bookManager->getLibraryBook(tafessirID);
        ml_return_on_fail(bookInfo && bookInfo->isTafessir() && m_viewManager->activeBook()->isQuran());

        int sora = m_viewManager->activeBookReader()->page()->sora;
        int aya = m_viewManager->activeBookReader()->page()->aya;

        BookViewBase *bookWidget = new BookWidget(bookInfo, this);
        m_viewManager->addBook(bookWidget);

        bookWidget->openSora(sora, aya);

        updateActions();
    } catch (BookException &e) {
        QMessageBox::warning(this,
                             tr("فتح التفسير"),
                             e.what());
    }
}

void BookReaderView::updateActions()
{
    if(m_viewManager->activeBookWidget()) {
        bool hasNext = m_viewManager->activeBookReader()->hasNext();
        bool hasPrev = m_viewManager->activeBookReader()->hasPrev();

        m_actionNextPage->setEnabled(hasNext);
        m_actionLastPage->setEnabled(hasNext);
        m_actionPrevPage->setEnabled(hasPrev);
        m_actionFirstPage->setEnabled(hasPrev);
    }
}

void BookReaderView::showIndexWidget()
{
    BookViewBase *book = m_viewManager->activeBookWidget();

    if(book)
        book->hideIndexWidget();
}

void BookReaderView::searchInBook()
{
    LibraryBook::Ptr book = m_viewManager->activeBook();
    ml_return_on_fail(book);

    MW->searchView()->newTab(SearchWidget::BookSearch, book->id);
    MW->showSearchView();
}

void BookReaderView::tabChanged(int newIndex)
{
    if(newIndex != -1) {
        updateActions();
        updateToolBars();

        if(m_viewManager->activeBook()->isQuran()) {
            m_actionNextAYA->setText(tr("الآية التالية"));
            m_actionPrevAYA->setText(tr("الآية السابقة"));
        } else {
            m_actionNextAYA->setText(tr("انزال الصفحة")); // TODO: fix this tooltip?
            m_actionPrevAYA->setText(tr("رفع الصفحة"));
        }
    }
}

void BookReaderView::loadTafessirList()
{
    m_comboTafasir->clear();
    m_comboTafasir->setModel(m_taffesirManager->taffesirListModel());

    m_comboTafasir->setEditable(true);
    m_comboTafasir->completer()->setCompletionMode(QCompleter::PopupCompletion);

    updateToolBars();
}

void BookReaderView::removeTashkil(bool remove)
{
    Utils::Settings::set("Style/removeTashekil", remove);

    QList<BookViewBase *> list = m_viewManager->getBookWidgets();
    foreach (BookViewBase *book, list) {
        book->bookReader()->setRemoveTashkil(remove);
        book->reloadCurrentPage();
    }
}

void BookReaderView::getSheer()
{
    LibraryBook::Ptr book = currentBook();
    ml_return_on_fail(book);
    ml_return_on_fail(currentBookWidget());

    AbstractBookReader *reader = currentBookWidget()->bookReader();
    ml_return_on_fail(reader);

    QuaZip zip;
    zip.setZipName(book->path);

    if(!zip.open(QuaZip::mdUnzip)) {
        qDebug() << "BookReaderView::getSheer Can't zip file" << book->path
                 << "Error:" << zip.getZipError();
        return;
    }

    QProgressDialog dialog(this);
    dialog.setWindowTitle(book->title);
    dialog.setLabelText(tr("جاري البحث عن الأبيات..."));
    dialog.setMaximum(reader->pagesCount());
    dialog.setMinimumDuration(0);
    dialog.setValue(0);
    dialog.setModal(true);
    dialog.show();

    QString tempDir = LibraryManager::instance()->libraryInfo()->tempDir();
    QString filePath = Utils::Rand::fileName(tempDir, true, "sheer_", "html");

    QFile outFile(filePath);
    if(!outFile.open(QIODevice::WriteOnly)) {
        qDebug() << "writePage::writeContent error when writing to " << filePath
                 << "Error:" << outFile.errorString();
        return;
    }

    QTextStream outStream(&outFile);
    outStream.setCodec("utf-8");

    outStream << "<html>"
                 "<head>"
                 "<title>" << book->title <<"</title>"
                 "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />" "\n"
                 "<style>"
                 "body { direction:rtl;";

    if(App::currentStyleName().toLower() == "black")
        outStream << "background-color:black; color: #ccc;";

    outStream << " }" "\n";
    outStream << "ul { list-style: decimal inside none; font-weight: bold;";

    if(App::currentStyleName().toLower() == "black")
        outStream << "color: #757DA7;";
    else
        outStream << "color: #273A9D;";

    outStream << "}";
    outStream << "a { border-bottom: 1px solid #CCCCCC; display: block; "
                 "font-size: 0.8em; text-align: left; text-decoration: none; "
                 "color: #7DA2FF; }";
    outStream << "a:visited { color: #7DA2FF; }";
    outStream << "</style>"
                 "<head>"
                 "<body>";

    int count = 0;
    int sheerCount = 0;

    BookPage page;
    QuaZipFileInfo info;
    QuaZipFile file(&zip);
    for(bool more=zip.goToFirstFile(); more; more=zip.goToNextFile()) {
        ml_return_on_fail2(zip.getCurrentFileInfo(&info),
                            "BookReaderView::getSheer getCurrentFileInfo Error" << zip.getZipError());

        int id = 0;
        QString name = info.name;
        if(name.startsWith("pages/p")) {
            name = name.remove(0, 7);
            name = name.remove(".html");

            bool ok;
            id = name.toInt(&ok);
            if(!ok) {
                qDebug("BookReaderView::getSheer can't convert '%s' to int", qPrintable(name));
                continue;
            }
        } else {
            continue;
        }

        if(!file.open(QIODevice::ReadOnly)) {
            qWarning("BookReaderView::getSheer zip error %d", zip.getZipError());
            continue;
        }

        QByteArray out;
        char buf[4096];
        int len = 0;

        while (!file.atEnd()) {
            len = file.read(buf, 4096);
            out.append(buf, len);

            if(len <= 0)
                break;
        }

        page.pageID = id;
        page.text = QString::fromUtf8(out);

        QWebPage webPage;
        webPage.mainFrame()->setHtml(page.text);

        QList<QWebElement> list = webPage.mainFrame()->findAllElements("sheer").toList();
        for(int i=0; i<list.count(); i++) {
            QWebElement element = list[i];
            QString html = "<ul>" "\n" "<li>" + element.toPlainText()+ "</li>"  "\n";

            QWebElement child = element.parent().nextSibling().firstChild();
            while(child.tagName().toLower() == "sheer") {
                html.append("<li>");
                html.append(child.toPlainText());
                html.append("</li>" "\n");

                list.removeAll(child);

                child = child.parent().nextSibling().firstChild();
            }

            html.append("</ul>");

            QWebElement paraElement = element.parent().previousSibling();

            html.prepend("<p>" "\n" + paraElement.toPlainText() + "\n");
            html.append("</p>" "\n");

            outStream << html;
        }

        if(list.count()) {
            outStream << QString("<a href=\"#\" onclick=\"bookView.openPageID(%1); return false\">%2 (%1)</a>")
                         .arg(id).arg(tr("فتح الصفحة"));
        }

        file.close();

        dialog.setValue(dialog.value()+1);

        ++sheerCount;
        if(++count > 100) {
            qApp->processEvents();
            count = 0;
        }

        if(dialog.wasCanceled())
            break;

        if(file.getZipError()!=UNZ_OK) {
            qWarning("BookReaderView::getSheer Unknow zip error %d", file.getZipError());
            continue;
        }
    }

    outStream << "</body>" "</html>";

    outStream.flush();
    outFile.close();

    QWebView *view = new QWebView();
    view->setWindowTitle(tr("%1 - عدد الأبيات الشعرية: %2").arg(book->title).arg(sheerCount));
    view->setUrl(QUrl::fromLocalFile(filePath));
    view->show();

    view->page()->mainFrame()->addToJavaScriptWindowObject("bookView", currentBookWidget());
}

void BookReaderView::editCurrentBook()
{
    LibraryBook::Ptr book = currentBook();
    BookPage *page = currentPage();

    if(book && page) {
        int pageID = page->pageID;

        try {
            MW->editorView()->editBook(book, pageID);
        } catch (BookException &e) {
            QMessageBox::information(this,
                                     App::name(),
                                     tr("حدث خطأ أثناء محاولة تحرير الكتاب الحالي:"
                                        "<br>%1").arg(e.what()));
        }
    }
}
