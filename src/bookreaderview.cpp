#include "bookreaderview.h"
#include "bookeditorview.h"
#include "bookslistbrowser.h"
#include "bookwidget.h"
#include "bookwidgetmanager.h"
#include "filterlineedit.h"
#include "indexwidget.h"
#include "librarybookmanager.h"
#include "libraryinfo.h"
#include "librarymanager.h"
#include "mainwindow.h"
#include "modelenums.h"
#include "openpagedialog.h"
#include "quranaudiomanager.h"
#include "richquranreader.h"
#include "richsimplebookreader.h"
#include "richtafessirreader.h"
#include "searchview.h"
#include "tabwidget.h"
#include "taffesirlistmanager.h"
#include "utils.h"
#include "webview.h"

#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>
#include <qaction.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qcompleter.h>
#include <qfile.h>
#include <qkeysequence.h>
#include <qmainwindow.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qprogressdialog.h>
#include <qsettings.h>
#include <qstackedwidget.h>
#include <qtoolbar.h>

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

    m_mediaObject = new Phonon::MediaObject(this);
    m_audioOutput = new Phonon::AudioOutput(this);
    m_handleSourceChange = false;

    Phonon::createPath(m_mediaObject, m_audioOutput);

    connect(m_viewManager, SIGNAL(currentTabChanged(int)), SLOT(updateActions()));
    connect(m_viewManager, SIGNAL(currentTabChanged(int)), SLOT(tabChanged(int)));
    connect(m_viewManager, SIGNAL(lastTabClosed()), SIGNAL(lastTabClosed()));
    connect(m_viewManager, SIGNAL(lastTabClosed()), SIGNAL(hideMe()));
    connect(m_mediaObject, SIGNAL(aboutToFinish()), SLOT(playerAboutToFinnish()));
    connect(m_mediaObject, SIGNAL(finished()), SLOT(playerFinnish()));
    connect(m_mediaObject, SIGNAL(currentSourceChanged(Phonon::MediaSource)),
            SLOT(playerSourceChanged(Phonon::MediaSource)));
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
    m_actionEditBook = new QAction(ml_theme_icon("document-edit", ":/images/document-edit.png"),
                                   tr("تحرير الكتاب"), this);
    // General Actions
    m_actionNewTab = new QAction(ml_theme_icon("tab-new", ":/images/tab-new.png"),
                                 tr("تبويب جديد"),
                                 this);

    m_actionIndexDock = new QAction(QIcon(":/images/edit_fahrass.png"),
                                    tr("نافذة الفهرس"),
                                    this);

    m_actionSearchInBook = new QAction(ml_theme_icon("edit-find", ":/images/find.png"),
                                       tr("بحث متقدم في هذا الكتاب"),
                                       this);

    // Navigation actions
    m_actionNextAYA = new QAction(ml_theme_icon("go-down", ":/images/go-down.png"),
                                  tr("الآية التالية"), this);

    m_actionNextPage = new QAction(ml_theme_icon("go-previous", ":/images/go-previous.png"),
                                   tr("الصفحة التالية"), this);

    m_actionPrevAYA = new QAction(ml_theme_icon("go-up", ":/images/go-up.png"),
                                  tr("الآية السابقة"), this);

    m_actionPrevPage = new QAction(ml_theme_icon("go-next", ":/images/go-next.png"),
                                   tr("الصفحة السابقة"), this);

    m_actionFirstPage = new QAction(ml_theme_icon("go-last", ":/images/go-last.png"),
                                    tr("الصفحة الاولى"), this);

    m_actionLastPage = new QAction(ml_theme_icon("go-first", ":/images/go-first.png"),
                                   tr("الصفحة الاخيرة"), this);

    m_actionGotToPage = new QAction(ml_theme_icon("go-jump", ":/images/go-jump.png"),
                                    tr("الصفحة..."), this);

    m_bookInfoAct = new QAction(tr("بطاقة الكتاب"), this);
    QAction *readHistoryAct = new QAction(tr("تاريخ تصفح الكتاب"), this);

    m_removeTashekilAct = new QAction(tr("حذف التشكيل"), this);
    m_removeTashekilAct->setCheckable(true);
    m_removeTashekilAct->setChecked(Utils::Settings::get("Style/removeTashekil", false).toBool());

    m_actionNextAYA->setShortcut(QKeySequence("J"));
    m_actionPrevAYA->setShortcut(QKeySequence("K"));
    m_actionNextPage->setShortcut(QKeySequence("N"));
    m_actionPrevPage->setShortcut(QKeySequence("P"));
    m_actionGotToPage->setShortcut(QKeySequence("G"));

    // Tafressir actions
    m_openSelectedTafsir =  new QAction(QIcon(":/images/arrow-left.png"),
                                        tr("فتح تفسير الاية"), this);

    m_comboTafasir = new QComboBox(this);

    m_playQuranAudio = new QAction(QIcon("/home/naruto/Programming/moltaqa-lib/share/moltaqa-lib/images/media-playback-start.png"),
                                   tr("قراءة الآية"), this);

    m_comboQuranReciter = new QComboBox(this);

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
    m_toolBarTafesir->addSeparator();
    m_toolBarTafesir->addWidget(m_comboQuranReciter);
    m_toolBarTafesir->addAction(m_playQuranAudio);

    m_toolBarGeneral->setObjectName("BookReaderView.General");
    m_toolBarNavigation->setObjectName("BookReaderView.Navigation");
    m_toolBarTafesir->setObjectName("BookReaderView.Tafesir");
    m_toolBarSearch->setObjectName("BookReaderView.Search");

    updateSearchNavigation();

    QMenu *navMenu = new QMenu(this);
    navMenu->setTitle(tr("انتقل إلى"));
    navMenu->addAction(m_actionFirstPage);
    navMenu->addAction(m_actionPrevPage);
    navMenu->addAction(m_actionNextPage);
    navMenu->addAction(m_actionLastPage);
    navMenu->addSeparator();
    navMenu->addAction(m_actionGotToPage);

    m_navActions << m_actionEditBook;
    m_navActions << actionSeparator(this);
    m_navActions << navMenu->menuAction();
    m_navActions << actionSeparator(this);
    m_navActions << m_removeTashekilAct;
    m_navActions << actionSeparator(this);
    m_navActions << m_bookInfoAct;
    m_navActions << readHistoryAct;

    m_toolBars << m_toolBarGeneral;
    m_toolBars << m_toolBarNavigation;
    m_toolBars << m_toolBarTafesir;
    m_toolBars << m_toolBarSearch;

    connect(m_viewManager, SIGNAL(pageChanged()), SLOT(updateActions()));
    connect(m_actionEditBook, SIGNAL(triggered()), SLOT(editCurrentBook()));

    QAction *getSheerAct = MW->getAction("search.all.sheer");
    if(getSheerAct)
        connect(getSheerAct, SIGNAL(triggered()), SLOT(getSheer()));

    QAction *getMateenAct = MW->getAction("search.all.mateen");
    if(getMateenAct)
        connect(getMateenAct, SIGNAL(triggered()), SLOT(getMateen()));

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
    connect(m_removeTashekilAct, SIGNAL(triggered(bool)), SLOT(removeTashkil(bool)));

    // Generale actions
    connect(m_actionIndexDock, SIGNAL(triggered()), SLOT(toggleIndexWidget()));
    connect(m_actionNewTab, SIGNAL(triggered()), MW, SLOT(showBooksList()));
    connect(m_actionSearchInBook, SIGNAL(triggered()), this, SLOT(searchInBook()));

    // Tafessir actions
    connect(m_openSelectedTafsir, SIGNAL(triggered()), SLOT(openTafessir()));
    connect(m_taffesirManager, SIGNAL(ModelsReady()), SLOT(loadTafessirList()));
    connect(m_playQuranAudio, SIGNAL(triggered()), SLOT(quranPlayToggle()));
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

    QString link = QString("moltaqa://?c=open&t=");
    if(bookReader->book()->isQuran())
        link.append(QString("quran&sora=%1&aya=%2").arg(bookReader->page()->sora).arg(bookReader->page()->aya));
    else
        link.append(QString("book&id=%1&page=%2").arg(bookReader->book()->uuid).arg(bookReader->page()->pageID));

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

BookWidgetManager *BookReaderView::bookWidgetManager()
{
    return m_viewManager;
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
                              e.format(true));
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
        QMessageBox::critical(this,
                              tr("فتح التفسير"),
                              e.format(true));
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

void BookReaderView::toggleIndexWidget()
{
    BookViewBase *book = m_viewManager->activeBookWidget();

    if(book)
        book->toggleIndexWidget();
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

    m_comboQuranReciter->clear();
    m_comboQuranReciter->setModel(m_libraryManager->quranAudioManager()->recitersModel());

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

void BookReaderView::quranPlayToggle()
{
    if(m_mediaObject->state() == Phonon::PlayingState) {
        m_mediaObject->pause();
    } else if(m_mediaObject->state() == Phonon::PausedState) {
        m_mediaObject->play();
    } else {
        playCurentAya();
        qDebug() << "Stat:" << m_mediaObject->state();
    }
}

void BookReaderView::playCurentAya()
{
    RichQuranReader *reader = qobject_cast<RichQuranReader*>(currentBookWidget()->bookReader());
    ml_return_on_fail2(reader, "BookReaderView::playCurentAya no Quran reader found");

    QString baseUrl = m_comboQuranReciter->itemData(m_comboQuranReciter->currentIndex()).toString();

    baseUrl.append(QString("/%1%2.mp3")
                   .arg(reader->page()->sora, 3, 10, QChar('0'))
                   .arg(reader->page()->aya, 3, 10, QChar('0')));

    qDebug() << "Play" << baseUrl;

    m_handleSourceChange = false;
    m_mediaObject->setCurrentSource(Phonon::MediaSource(baseUrl));
    m_mediaObject->play();

    m_handleSourceChange = true;
}

void BookReaderView::playerAboutToFinnish()
{
    RichQuranReader *reader = qobject_cast<RichQuranReader*>(currentBookWidget()->bookReader());
    ml_return_on_fail2(reader, "BookReaderView::playCurentAya no Quran reader found");

    QString baseUrl = m_comboQuranReciter->itemData(m_comboQuranReciter->currentIndex()).toString();

    int sora = reader->page()->sora;
    int aya = reader->page()->aya + 1;
    if(aya > reader->page()->ayatCount) {
        aya = 1;
        ++sora;
    }

    baseUrl.append(QString("/%1%2.mp3")
                   .arg(sora, 3, 10, QChar('0'))
                   .arg(aya, 3, 10, QChar('0')));

    qDebug() << "Enqueue" << baseUrl;

    m_mediaObject->enqueue(Phonon::MediaSource(baseUrl));
}

void BookReaderView::playerFinnish()
{
    //currentBookWidget()->bookReader()->nextAya();
}

void BookReaderView::playerSourceChanged(const Phonon::MediaSource &source)
{
    if (m_handleSourceChange) {
        qDebug() << "BookReaderView::playerSourceChanged" << source.url().path();
        currentBookWidget()->scrollDown();
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

    QSettings settings;
    settings.beginGroup("DefaultFont");

    QString fontString = settings.value("fontFamily").toString();
    int fontSize = settings.value("fontSize").toInt();

    QString ulColor;
    QString bodyColor;
    QString aBorderColor;
    if(App::currentStyleName().toLower() == "black") {
        bodyColor = "background-color:black; color: #ccc;";
        ulColor = "#757DA7";
        aBorderColor = "#555555";
    } else {
        ulColor = "#273A9D";
        aBorderColor = "#AAAAAA";
    }

    QTextStream outStream(&outFile);
    outStream.setCodec("utf-8");

    outStream << "<html>"
                 "<head>"
                 "<title>" << book->title <<"</title>"
                 "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />" "\n"
                 "<style>" "\n";

    outStream << "body { direction:rtl;" << bodyColor
              << "font-family: '" << fontString << "'; "
              << "font-size: " << fontSize << "px;" << " }" "\n";

    outStream << "ul { list-style: decimal inside none; font-weight: bold; "
                 "color: " << ulColor << "}" "\n";

    outStream << "a { border-bottom: 1px solid " << aBorderColor << ";"
              << "display: block; font-size: 0.8em; text-align: left; "
              << "text-decoration: none; color: #7DA2FF; }" "\n";

    outStream << "a:visited { color: #7DA2FF; }" "\n";

    outStream << "</style>"
                 "<head>"
                 "<body>" "\n";

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
        if(name.startsWith(QLatin1String("pages/p"))) {
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
        if(!Utils::Files::copyData(file, out))
            break;

        page.pageID = id;
        page.text = QString::fromUtf8(out);

        QWebPage webPage;
        webPage.mainFrame()->setHtml(page.text);

        QList<QWebElement> list = webPage.mainFrame()->findAllElements("sheer").toList();
        for(int i=0; i<list.count(); i++) {
            QWebElement element = list[i];
            QString html = "<ul>" "\n" "<li>" + element.toPlainText()+ "</li>"  "\n";
            ++sheerCount;

            QWebElement child = element.parent().nextSibling().firstChild();
            while(child.tagName().toLower() == "sheer") {
                html.append("<li>");
                html.append(child.toPlainText());
                html.append("</li>" "\n");

                list.removeAll(child);
                ++sheerCount;

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

    if(sheerCount) {
        QDialog *dialog = new QDialog(0);
        dialog->setWindowIcon(windowIcon());
        dialog->setWindowTitle(tr("%1 - عدد الأبيات الشعرية: %2").arg(book->title).arg(sheerCount));

        WebView *view = new WebView(dialog);
        view->autoObjectAdd("bookView", currentBookWidget());
        view->setUrl(QUrl::fromLocalFile(filePath));

        QVBoxLayout *layout = new QVBoxLayout;
        layout->setMargin(0);
        layout->addWidget(view);

        dialog->setLayout(layout);
        dialog->show();
    } else {
        QMessageBox::warning(this,
                             book->title,
                             tr("لم يتم العثور على أي بيت شعري في هذا الكتاب!"));

        QFile::remove(filePath);
    }
}

void BookReaderView::getMateen()
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
    dialog.setLabelText(tr("جاري البحث عن المتون..."));
    dialog.setMaximum(reader->pagesCount());
    dialog.setMinimumDuration(0);
    dialog.setValue(0);
    dialog.setModal(true);
    dialog.show();

    QString tempDir = LibraryManager::instance()->libraryInfo()->tempDir();
    QString filePath = Utils::Rand::fileName(tempDir, true, "mateen_", "html");

    QFile outFile(filePath);
    if(!outFile.open(QIODevice::WriteOnly)) {
        qDebug() << "writePage::writeContent error when writing to " << filePath
                 << "Error:" << outFile.errorString();
        return;
    }

    QSettings settings;
    settings.beginGroup("DefaultFont");

    QString fontString = settings.value("fontFamily").toString();
    int fontSize = settings.value("fontSize").toInt();

    QString ulColor;
    QString bodyColor;
    QString aBorderColor;
    if(App::currentStyleName().toLower() == "black") {
        bodyColor = "background-color:black; color: #ccc;";
        ulColor = "#757DA7";
        aBorderColor = "#555555";
    } else {
        ulColor = "#273A9D";
        aBorderColor = "#AAAAAA";
    }

    QTextStream outStream(&outFile);
    outStream.setCodec("utf-8");

    outStream << "<html>"
                 "<head>"
                 "<title>" << book->title <<"</title>"
                 "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />" "\n"
                 "<style>" "\n";

    outStream << "body { direction:rtl;" << bodyColor
              << "font-family: '" << fontString << "'; "
              << "font-size: " << fontSize << "px;" << " }" "\n";

    outStream << "mateen {font-weight: bold; color: " << ulColor << "}" "\n";

    outStream << "a { border-bottom: 1px solid " << aBorderColor << ";"
              << "display: block; font-size: 0.8em; text-align: left; "
              << "text-decoration: none; color: #7DA2FF; }" "\n";

    outStream << "a:visited { color: #7DA2FF; }" "\n";

    outStream << "</style>"
                 "<head>"
                 "<body>" "\n";

    int count = 0;
    int mateenCount = 0;

    BookPage page;
    QuaZipFileInfo info;
    QuaZipFile file(&zip);
    for(bool more=zip.goToFirstFile(); more; more=zip.goToNextFile()) {
        ml_return_on_fail2(zip.getCurrentFileInfo(&info),
                           "BookReaderView::getSheer getCurrentFileInfo Error" << zip.getZipError());

        int id = 0;
        QString name = info.name;
        if(name.startsWith(QLatin1String("pages/p"))) {
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
        if(!Utils::Files::copyData(file, out))
            break;

        page.pageID = id;
        page.text = QString::fromUtf8(out);

        QWebPage webPage;
        webPage.mainFrame()->setHtml(page.text);

        QList<QWebElement> list = webPage.mainFrame()->findAllElements("mateen").toList();
        for(int i=0; i<list.count(); i++) {
            QWebElement element = list[i];

            QString html = "<p>" "\n";
            html.append("<mateen>");
            html.append(element.toPlainText());
            html.append("</mateen>");

            html.append("</p>" "\n");

            ++mateenCount;
            outStream << html;
        }

        if(list.count()) {
            outStream << QString("<a href=\"#\" onclick=\"bookView.openPageID(%1); return false\">%2 (%1)</a>")
                         .arg(id).arg(tr("فتح الصفحة"));
        }

        file.close();

        dialog.setValue(dialog.value()+1);

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

    if(mateenCount) {
        QDialog *dialog = new QDialog(0);
        dialog->setWindowIcon(windowIcon());
        dialog->setWindowTitle(tr("%1 - عدد المتون: %2").arg(book->title).arg(mateenCount));

        WebView *view = new WebView(dialog);
        view->autoObjectAdd("bookView", currentBookWidget());
        view->setUrl(QUrl::fromLocalFile(filePath));

        QVBoxLayout *layout = new QVBoxLayout;
        layout->setMargin(0);
        layout->addWidget(view);

        dialog->setLayout(layout);
        dialog->show();
    } else {
        QMessageBox::warning(this,
                             book->title,
                             tr("لم يتم العثور على أي  متن في هذا الكتاب!"));

        QFile::remove(filePath);
    }
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
            QMessageBox::warning(this,
                                 App::name(),
                                 tr("حدث خطأ أثناء محاولة تحرير الكتاب الحالي:"
                                    "<br>%1").arg(e.what()));
        }
    }
}
