#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "authorsview.h"
#include "bookeditorview.h"
#include "bookreaderhelper.h"
#include "bookreaderview.h"
#include "bookwidget.h"
#include "checkablemessagebox.h"
#include "controlcenterdialog.h"
#include "exportdialog.h"
#include "importdialog.h"
#include "indexmanager.h"
#include "indextracker.h"
#include "libraryinfo.h"
#include "librarymanager.h"
#include "logdialog.h"
#include "newlibrarydialog.h"
#include "searchview.h"
#include "settingsdialog.h"
#include "shamelaimportdialog.h"
#include "tarajemrowatview.h"
#include "updatedialog.h"
#include "utils.h"
#include "viewmanager.h"
#include "webview.h"
#include "welcomewidget.h"

#include <qevent.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qprogressbar.h>
#include <qsettings.h>
#include <qtextbrowser.h>
#include <qtimer.h>
#include <qtoolbutton.h>
#include <qwebsettings.h>

static MainWindow *m_instance = 0;

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_viewManager(0),
    m_libraryInfo(0),
    m_libraryManager(0),
    m_bookView(0),
    m_booksList(0),
    m_welcomeWidget(0),
    m_indexTracker(0),
    m_indexManager(0),
    m_indexBar(0),
    m_searchView(0),
    m_readerHelper(0),
    m_editorView(0),
    m_tarajemView(0),
    m_authorsView(0)
{
    ui->setupUi(this);
    ml_set_instance(m_instance, this);

    m_logDialog = new LogDialog(this);
    m_updateChecker = new UpdateChecker(this);

    setWindowTitle(App::name());
    setupProgressWidget();
    loadSettings();

    connect(ui->actionLogDialog, SIGNAL(triggered()), SLOT(showLogDialog()));
    connect(ui->actionHelp, SIGNAL(triggered()), SLOT(showHelp()));
    connect(ui->actionFullScreen, SIGNAL(triggered()), SLOT(fullScreenMode()));
    connect(ui->actionUpdate, SIGNAL(triggered()), m_updateChecker, SLOT(startCheck()));
    connect(m_updateChecker, SIGNAL(checkFinished()), SLOT(checkFinnished()));
}

bool MainWindow::init()
{
    // TODO: re-code this properly
    QString libDir = Utils::Settings::get("library_dir").toString();
    QString message;

    if(!Utils::Library::isValidLibrary(libDir)) {
        if(libDir.size()) {
            // We have a path to the library but it is invalid
            message = tr("لم يتم العثور على المكتبة في المجلد:") + QDir(libDir).absolutePath() + "<br>";
        } else {
            message = tr("لم تقم بانشاء مكتبة بعد،") + " ";
        }

        libDir = createLibrary(message);

        ml_return_val_on_fail(libDir.size(), false);
    }

    try {
        m_libraryInfo = new LibraryInfo();
        m_libraryInfo->load(libDir);
    } catch(BookException &e) {
        QMessageBox::critical(this,
                              App::name(),
                              tr("حدث خطأ أثناء فتح المكتبة" "<br>%1<br>"
                                 "من فضلك أعد تشغيل البرنامج").arg(e.format(true)));

        Utils::Settings::remove("library_dir");

        return false;
    }

    m_viewManager = new ViewManager(this);
    setCentralWidget(m_viewManager);

    try {

        m_libraryManager = new LibraryManager(m_libraryInfo);

        m_booksList = new BooksListBrowser(0);
        m_booksList->setWindowIcon(windowIcon());

        // IndexTracker should be created before the IndexManager
        m_indexTracker = new IndexTracker(this);
        m_indexManager = new IndexManager(this);

        m_welcomeWidget = new WelcomeWidget(this);
        m_viewManager->addView(m_welcomeWidget);
        m_viewManager->setDefautView(m_welcomeWidget);
        m_viewManager->setCurrentView(m_welcomeWidget);

        m_bookView = new BookReaderView(m_libraryManager, this);
        m_viewManager->addView(m_bookView, false);

        m_searchView = new SearchView(this);
        m_viewManager->addView(m_searchView, false);

        m_editorView = new BookEditorView(this);
        m_viewManager->addView(m_editorView, false);

        m_tarajemView = new TarajemRowatView(this);
        m_viewManager->addView(m_tarajemView, false);

        m_authorsView = new AuthorsView(this);
        m_viewManager->addView(m_authorsView, false);

        setupActions();

        m_readerHelper = new BookReaderHelper();
        m_indexTracker->findTasks();

        m_libraryManager->addHelpBook();

    } catch(BookException &e) {
        QMessageBox::critical(this,
                              App::name(),
                              tr("حدث خطأ أثناء تحميل البرنامج:" "<br>"
                                 "%1").arg(e.format(true)));
        return false;
    }

    return true;
}

MainWindow::~MainWindow()
{
    ml_delete_check(m_booksList);
    ml_delete_check(m_welcomeWidget);
    ml_delete_check(m_bookView);
    ml_delete_check(m_searchView);
    ml_delete_check(m_libraryManager);
    ml_delete_check(m_libraryInfo);
    ml_delete_check(m_readerHelper);
    ml_delete_check(m_tarajemView);
    ml_delete_check(m_authorsView);
    ml_delete_check(m_indexTracker);
    ml_delete_check(m_logDialog);

    delete ui;

    m_instance = 0;
}

MainWindow *MainWindow::instance()
{
    Q_CHECK_PTR(m_instance);

    return m_instance;
}

void MainWindow::setupActions()
{

    connect(ui->actionExit, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
    connect(ui->actionAbout, SIGNAL(triggered()), SLOT(aboutdDialog()));
    connect(ui->actionSettings, SIGNAL(triggered()), SLOT(settingDialog()));
    connect(ui->actionControlCenter, SIGNAL(triggered()), SLOT(controlCenter()));
    connect(ui->actionLibraryInfo, SIGNAL(triggered()), SLOT(showLibraryInfo()));
    connect(ui->actionImport, SIGNAL(triggered()), SLOT(importBookDialog()));
    connect(ui->actionShamelaImport, SIGNAL(triggered()), SLOT(importFromShamela()));
    connect(ui->actionExport, SIGNAL(triggered()), SLOT(exportBooks()));

    connect(m_booksList, SIGNAL(bookSelected(int,int)), SLOT(openBook(int,int)));
    connect(ui->actionBooksList, SIGNAL(triggered()), SLOT(showBooksList()));
    connect(ui->actionSearchView, SIGNAL(triggered()), SLOT(showSearchView()));
    connect(ui->actionSearchFavourites, SIGNAL(triggered()), SLOT(searchInFavourites()));
    connect(ui->actionSearchInBook, SIGNAL(triggered()), m_bookView, SLOT(searchInBook()));
    connect(ui->actionTarajemRowat, SIGNAL(triggered()), SLOT(showTarajemRowatView()));
    connect(ui->actionAuthorsView, SIGNAL(triggered()), SLOT(showAuthorsView()));

    connect(m_indexManager, SIGNAL(progress(int,int)), SLOT(indexProgress(int,int)));
    connect(m_indexManager, SIGNAL(started()), SLOT(indexingStart()));
    connect(m_indexManager, SIGNAL(done()), SLOT(indexingStop()));

    if(Utils::Settings::get("Search/autoUpdateIndex", true).toBool())
        connect(m_indexTracker, SIGNAL(gotTask()), m_indexManager, SLOT(start()));
}

void MainWindow::aboutdDialog()
{
    AboutDialog dialog(this);
    Utils::Widget::exec(&dialog, "AboutDialog");
}

void MainWindow::settingDialog()
{
    SettingsDialog settingDialog(this);
    Utils::Widget::exec(&settingDialog, "SettingDialog");
}

void MainWindow::openBook(int bookID, int pageID)
{
    m_bookView->openBook(bookID, pageID);
}

void MainWindow::showBooksList()
{
    m_booksList->show();
    m_booksList->activateWindow();
}

void MainWindow::showBooksList(BooksListBrowser::TreeViewModel model)
{
    showBooksList();
    m_booksList->setCurrentModel(model);
}

void MainWindow::showSearchView()
{
    if(m_searchView->canSearch())
        m_viewManager->setCurrentView(m_searchView);
}

void MainWindow::showTarajemRowatView()
{
    m_viewManager->setCurrentView(m_tarajemView);
}

void MainWindow::showAuthorsView()
{
    m_viewManager->setCurrentView(m_authorsView);
}

void MainWindow::searchInFavourites()
{
    m_searchView->newTab(SearchWidget::FavouritesSearch);
    m_viewManager->setCurrentView(m_searchView);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int ret = CheckableMessageBox::question(this, windowTitle(),
                                            tr("هل تريد إغلاق البرنامج؟"),
                                            "showCloseWarning",
                                            QDialogButtonBox::Yes);
    if(ret == QDialogButtonBox::No) {
        event->ignore();
        return;
    }

    if(!isFullScreen())
        Utils::Widget::save(this, "MainWindow");

    m_booksList->close();
    m_editorView->maySave(false);

    if(m_indexManager->isIndexing())
        m_indexManager->stop();

    m_viewManager->aboutToClose();

    event->accept();
}

QString MainWindow::createLibrary(const QString &messageText)
{
    QString libDir;

    int ret = QMessageBox::question(this,
                                App::name(),
                                messageText +
                                tr("ما الذي تريد القيام به؟"),
                                tr("تغيير مجلد المكتبة"), tr("انشاء مكتبة مفرغة"), tr("خروج"),
                                2);
    if(ret == 2) {
        // Exist
        return QString();
    } else if(ret == 1){
        // Create new empty library
        NewLibraryDialog dialog(this);
        if(dialog.exec() == QDialog::Accepted) {
            libDir = dialog.libraryDir();
            Utils::Settings::set("library_dir", libDir);
        } else {
            QMessageBox::critical(this,
                                  App::name(),
                                  tr("لم يتم انشاء المكتبة بشكل صحيح" "\n"
                                     "من فضلك أعد تشغيل البرنامج وحاول انشاء المكتبة من جديد"));
            return QString();
        }
    } else if(ret == 0) {
        // Update library path
        QString path = QFileDialog::getExistingDirectory(this,
                                                         tr("اختر مجلد المكتبة"),
                                                         QDir::homePath());
        if(path.size() && Utils::Library::isValidLibrary(path)) {
            libDir = path;
            Utils::Settings::set("library_dir", libDir);
        } else {
            QMessageBox::critical(this,
                                  App::name(),
                                  tr("لم يتم تحديث مجلد المكتبة بشكل صحيح" "\n"
                                     "من فضلك أعد تشغيل البرنامج وحاول  تحديث مجلد المكتبة من جديد"));
            return QString();

        }
    }

    return libDir;
}

void MainWindow::setupProgressWidget()
{
    m_progressWidget = new QWidget(statusBar());
    m_progressWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    m_indexBar = new QProgressBar(m_progressWidget);
    m_indexBar->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_indexBar->setMaximumWidth(200);
    m_indexBar->setFormat("%v/%m");
    m_indexBar->setAlignment(Qt::AlignCenter);
    m_indexBar->setToolTip(tr("تقدم تحديث الفهرس"));

    m_indexStopButton = new QToolButton(m_progressWidget);
    m_indexStopButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_indexStopButton->setIcon(QIcon(":/images/stop.png"));
    m_indexStopButton->setIconSize(QSize(16, 10));
    m_indexStopButton->setAutoRaise(true);
    m_indexStopButton->setToolTip(tr("إيقاف تحديث الفهرس"));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->addStretch();
    layout->addWidget(m_indexBar);
    layout->addWidget(m_indexStopButton);

    m_progressWidget->setLayout(layout);
    m_indexBar->hide();
    m_indexStopButton->hide();

    connect(m_indexStopButton, SIGNAL(clicked()), SLOT(stopIndexing()));

    statusBar()->addPermanentWidget(m_progressWidget);
}

void MainWindow::loadSettings()
{
    Utils::Widget::restore(this, "MainWindow", true);

    SettingsDialog::checkDefaultFonts();

    QSettings settings;
    if(settings.value("Update/autoCheck", true).toBool()) {
        uint current = QDateTime::currentDateTime().toTime_t();
        uint lastCheck = settings.value("Update/lastCheck", 0).toUInt();
        if(current - lastCheck > 43200)
            QTimer::singleShot(30000, this, SLOT(autoUpdateCheck()));
    }
}

void MainWindow::handleMessage(const QString &)
{
}

void MainWindow::stopIndexing()
{
    Q_CHECK_PTR(m_indexManager);

    m_indexManager->stop();
}

LibraryInfo *MainWindow::libraryInfo()
{
    Q_CHECK_PTR(m_libraryInfo);

    return m_libraryInfo;
}

LibraryManager *MainWindow::libraryManager()
{
    Q_CHECK_PTR(m_libraryInfo);

    return m_libraryManager;
}

BookReaderView *MainWindow::bookReaderView()
{
    return m_bookView;
}

BooksListBrowser *MainWindow::booksListBrowser()
{
    return m_booksList;
}

IndexTracker *MainWindow::indexTracker()
{
    Q_CHECK_PTR(m_indexTracker);

    return m_indexTracker;
}

IndexManager * MainWindow::indexManager()
{
    Q_CHECK_PTR(m_indexManager);

    return m_indexManager;
}

BookReaderHelper *MainWindow::readerHelper()
{
    Q_CHECK_PTR(m_readerHelper);

    return m_readerHelper;
}

SearchView *MainWindow::searchView()
{
    Q_CHECK_PTR(m_searchView);

    return m_searchView;
}

BookEditorView *MainWindow::editorView()
{
    Q_CHECK_PTR(m_editorView);

    return m_editorView;
}

QAction *MainWindow::getAction(QString actionId)
{
    if(actionId == "search.all.sheer")
        return ui->actionSearchAllSheer;
    else if (actionId == "search.all.mateen")
        return ui->actionSearchAllMateen;

#ifdef DEV_BUILD
    qWarning() << "MainWindow::getAction no Action found for:" << actionId;
#endif
        return 0;
}

QMenu *MainWindow::getMenu(QString menuId)
{
    if(menuId == "menu.navigation")
        return ui->menuNavigation;
    else if( menuId == "menu.windows")
        return ui->menuWindows;

#ifdef DEV_BUILD
    qWarning() << "MainWindow::getMenu no Menu found for:" << menuId;
#endif

    return 0;
}

void MainWindow::controlCenter()
{
    ControlCenterDialog dialog(this);
    dialog.exec();
}

void MainWindow::importBookDialog()
{
    ImportDialog *dialog = new ImportDialog(0);
    dialog->setWindowIcon(windowIcon());
    dialog->show();

    connect(dialog, SIGNAL(openBook(int)), this, SLOT(openBook(int)));
}

void MainWindow::importFromShamela()
{
    ShamelaImportDialog importDialog;
    importDialog.setWindowIcon(windowIcon());
    importDialog.exec();
}

void MainWindow::exportBooks()
{
    ExportDialog exportDialog;
    exportDialog.setWindowIcon(windowIcon());
    exportDialog.exec();
}

void MainWindow::indexingStart()
{
    m_indexBar->show();
    m_indexStopButton->show();
}

void MainWindow::indexingStop()
{
    m_indexBar->hide();
    m_indexStopButton->hide();
}

void MainWindow::indexProgress(int value, int max)
{
    m_indexBar->setMaximum(max);
    m_indexBar->setValue(value);
}

void MainWindow::showLogDialog()
{
    m_logDialog->startWatching();
    m_logDialog->show();
}

#define NEW_LINE() textBrowser->append("<br/>");
#define ADD_VALUE(name, value) \
    textBrowser->append( \
    QString("<strong>%1:</strong> <strong style=\"color:green\">%2</strong>") \
    .arg(tr(name)).arg(value));


void MainWindow::showLibraryInfo()
{
        QTextBrowser *textBrowser = new QTextBrowser;

        ADD_VALUE("اسم المكتبة", m_libraryInfo->name());
        ADD_VALUE("مسار المكتبة", m_libraryInfo->path());
        NEW_LINE();

        ADD_VALUE("حجم كل المكتبة", Utils::Files::formatSize(Utils::Files::directorySize(m_libraryInfo->path())));
        ADD_VALUE("حجم الكتب", Utils::Files::formatSize(Utils::Files::directorySize(m_libraryInfo->booksDir())));
        ADD_VALUE("حجم الفهرس", Utils::Files::formatSize(Utils::Files::directorySize(m_libraryInfo->indexDataDir())));
        NEW_LINE();

        QHash<QString, QVariant> s = LibraryManager::instance()->libraryStatistic();
        int booksCount = s["books_count"].toInt();
        int categorieCount = s["categories_count"].toInt();
        int authorsCount = s["authors_count"].toInt();
        int rowatCount = s["rowat_count"].toInt();

        if(booksCount)   ADD_VALUE("عدد الكتب", booksCount);
        if(categorieCount)   ADD_VALUE("عدد الأقسام", categorieCount);
        if(authorsCount) ADD_VALUE("تراجم المؤلفين", authorsCount);
        if(rowatCount)   ADD_VALUE("تراجم الرواة", rowatCount);

        QDialog *dialog = new QDialog(this);
        Utils::Widget::hideHelpButton(dialog);

        QVBoxLayout *layout = new QVBoxLayout();
        QLabel *label = new QLabel(tr("معلومات حول المكتبة:"), dialog);
        layout->addWidget(label);
        layout->addWidget(textBrowser);

        dialog->setWindowTitle(tr("%1 %2").arg(App::name()).arg(App::version()));
        dialog->setLayout(layout);
        dialog->resize(350, 250);
        dialog->show();
}

void MainWindow::showHelp()
{
    openBook(LibraryManager::helpBookID());
}

void MainWindow::checkFinnished()
{
    UpdateInfo *info = m_updateChecker->result();

    if(!info) {
        if(m_updateChecker->autoCheck) {
            QSettings settings;
            settings.setValue("Update/lastCheck", QDateTime::currentDateTime().toTime_t());
        } else {
            if(m_updateChecker->hasError) {
                QMessageBox::warning(this,
                                     tr("تحديث البرنامج"),
                                     tr("حدث خطأ اثناء البحث عن التحديث:" "\n")
                                     + m_updateChecker->errorString);
            } else {
                QMessageBox::information(this,
                                         tr("تحديث البرنامج"),
                                         tr("لا يوجد تحديث للبرنامج، انت تستخدم اخر اصدار"));
            }
        }
    } else {
        UpdateDialog dialog(this);
        dialog.setDownloadUrl(info);
        dialog.exec();
    }
}


void MainWindow::autoUpdateCheck()
{
    m_updateChecker->startCheck(true);
}

void MainWindow::fullScreenMode()
{
    if(isFullScreen()) {
        statusBar()->setVisible(true);
        showNormal();
        Utils::Widget::restore(this, "MainWindow", true);
    } else {
        Utils::Widget::save(this, "MainWindow");
        statusBar()->setVisible(false);
        showFullScreen();
    }
}
