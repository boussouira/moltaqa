#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include "booksviewer.h"
#include "librarymanager.h"
#include "bookslistbrowser.h"
#include "settingsdialog.h"
#include "bookwidget.h"
#include "importdialog.h"
#include "bookexception.h"
#include "libraryinfo.h"
#include "welcomewidget.h"
#include "shamelaimportdialog.h"
#include "newlibrarydialog.h"
#include "controlcenterdialog.h"
#include "indextracker.h"
#include "indexmanager.h"
#include "viewmanager.h"
#include "bookreaderhelper.h"

#include <qmessagebox.h>
#include <qsettings.h>
#include <qevent.h>
#include <qfile.h>
#include <qtimer.h>
#include <qfiledialog.h>
#include <qprogressbar.h>

static MainWindow *m_mainWindow = 0;

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_libraryInfo(0),
    m_libraryManager(0),
    m_indexTracker(0),
    m_indexManager(0),
    m_readerHelper(0)
{
    ui->setupUi(this);
    m_mainWindow = this;

    m_welcomeWidget = 0;
    m_bookView = 0;
    m_searchView = 0;

    setWindowTitle(App::name());
    loadSettings();
}

bool MainWindow::init()
{
    // TODO: re-code this properly
    QSettings settings;
    QString libDir = settings.value("library_dir").toString();

    if(!Utils::isLibraryPath(libDir)) {
        int ret;
        if(!libDir.isEmpty()) {
            // We have a path to the library but it is invalid
            ret = QMessageBox::question(this,
                                        App::name(),
                                        tr("لم يتم العثور على المكتبة في المجلد:" "<br>")
                                        + QDir(libDir).absolutePath() + "<br>" +
                                        tr("ما الذي تريد القيام به؟"),
                                        tr("تحديث مجلد المكتبة"), tr("انشاء مكتبة مفرغة"), tr("خروج"),
                                        2);
        } else {
            // We should create a empty library
            ret = 1;
        }

        if(ret == 2) {
            // Exist
            return false;
        } else if(ret == 1){
            // Create new empty library
            NewLibraryDialog dialog(this);
            if(dialog.exec() == QDialog::Accepted) {
                libDir = dialog.libraryDir();
                settings.setValue("library_dir", libDir);
            } else {
                QMessageBox::critical(this,
                                      App::name(),
                                      tr("لم يتم انشاء المكتبة بشكل صحيح" "\n"
                                         "من فضلك أعد تشغيل البرنامج وحاول انشاء المكتبة من جديد"));
                return false;
            }
        } else if(ret == 0) {
            // Update library path
            QString path = QFileDialog::getExistingDirectory(this,
                                                             tr("اختر مجلد المكتبة"),
                                                             QDir::homePath());
            if(!path.isEmpty() && Utils::isLibraryPath(path)) {
                libDir = path;
                settings.setValue("library_dir", libDir);
            } else {
                QMessageBox::critical(this,
                                      App::name(),
                                      tr("لم يتم تحديث مجلد المكتبة بشكل صحيح" "\n"
                                         "من فضلك أعد تشغيل البرنامج وحاول  تحديث مجلد المكتبة من جديد"));
                return false;

            }
        }
    }

    m_viewManager = new ViewManager(this);
    m_viewManager->setMenu(ui->menuWindow);
    setCentralWidget(m_viewManager);

    m_welcomeWidget = new WelcomeWidget(this);
    m_viewManager->addView(m_welcomeWidget);
    m_viewManager->setDefautView(m_welcomeWidget);
    m_viewManager->setCurrentView(m_welcomeWidget);

    try {
        m_libraryInfo = new LibraryInfo(libDir);

        m_libraryManager = new LibraryManager(m_libraryInfo);
        m_libraryManager->open();
        m_libraryManager->loadBooksListModel();

        m_bookView = new BooksViewer(m_libraryManager, this);
        m_viewManager->addView(m_bookView, false);

        m_booksList = new BooksListBrowser(m_libraryManager, 0);

        // IndexTracker should be created before the IndexManager
        m_indexTracker = new IndexTracker(this);
        m_indexManager = new IndexManager(this);

        m_indexBar = new QProgressBar(statusBar());
        m_indexBar->setMaximumWidth(200);
        m_indexBar->setFormat("");
        m_indexBar->setToolTip(tr("تقدم تحديث الفهرس"));
        m_indexBar->hide();
        statusBar()->addPermanentWidget(m_indexBar);

        m_searchView = new SearchView(this);
        m_viewManager->addView(m_searchView, false);
//        showSearchView();

        setupActions();

        m_readerHelper = new BookReaderHelper();

        m_indexTracker->findTasks();
    } catch(BookException &e) {
        QMessageBox::information(this,
                                 App::name(),
                                 tr("حدث خطأ أثناء تحميل البرنامج:"
                                    "<br>%1").arg(e.what()));
        return false;
    }

    return true;
}

MainWindow::~MainWindow()
{
    if(m_welcomeWidget)
        delete m_welcomeWidget;

    if(m_bookView)
        delete m_bookView;

    if(m_searchView)
        delete m_searchView;

    if(m_libraryManager)
        delete m_libraryManager;

    if(m_libraryInfo)
        delete m_libraryInfo;

    if(m_readerHelper)
        delete m_readerHelper;

    if(m_indexTracker)
        delete m_indexTracker;

    delete ui;

    m_mainWindow = 0;
}

MainWindow *MainWindow::mainWindow()
{
    Q_ASSERT(m_mainWindow);

    return m_mainWindow;
}

void MainWindow::setupActions()
{

    connect(ui->actionExit, SIGNAL(triggered()), SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), SLOT(aboutApp()));
    connect(ui->actionSettings, SIGNAL(triggered()), SLOT(settingDialog()));
    connect(ui->actionControlCenter, SIGNAL(triggered()), SLOT(controlCenter()));

    //TODO: open Quran quickly
    connect(m_welcomeWidget, SIGNAL(showBooksList()), SLOT(showBooksList()));
    connect(m_bookView, SIGNAL(lastTabClosed()), SLOT(lastTabClosed()));
    connect(m_booksList, SIGNAL(bookSelected(int)), SLOT(openBook(int)));
    connect(ui->actionBooksList, SIGNAL(triggered()), SLOT(showBooksList()));
    connect(ui->actionSearchView, SIGNAL(triggered()), SLOT(showSearchView()));

    connect(m_indexManager, SIGNAL(progress(int,int)), SLOT(indexProgress(int,int)));
    connect(m_indexManager, SIGNAL(started()), SLOT(startIndexing()));
    connect(m_indexManager, SIGNAL(done()), SLOT(stopIndexing()));
    connect(m_indexTracker, SIGNAL(gotTask()), m_indexManager, SLOT(start()));
}

void MainWindow::aboutApp()
{
    QMessageBox::information(this,
                             App::name(),
                             tr("برنامج الكتبية لقراءة القرءان الكريم"));
}

void MainWindow::settingDialog()
{
    SettingsDialog settingDialog(this);
    settingDialog.exec();
}

void MainWindow::quranWindow()
{
    openBook(defaultQuran);
}

void MainWindow::openBook(int pBookID)
{
    try {
        m_bookView->openBook(pBookID);

        if(m_bookView->isHidden())
            m_bookView->show();

        m_bookView->setSelectable(true);
        m_viewManager->setCurrentView(m_bookView);

    } catch(BookException &e) {
        QMessageBox::information(this,
                                 App::name(),
                                 tr("حدث خطأ أثناء فتح الكتاب:"
                                    "<br>%1").arg(e.what()));
    }
}

void MainWindow::showBooksList()
{
    m_booksList->show();
    m_booksList->activateWindow();
}

void MainWindow::showSearchView()
{
    m_searchView->setSelectable(true);
    m_searchView->ensureTabIsOpen();
    m_viewManager->setCurrentView(m_searchView);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("maximized", isMaximized());
    settings.endGroup();

    m_booksList->close();
    m_indexManager->stop();

    event->accept();
}

void MainWindow::loadSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    move(settings.value("pos", pos()).toPoint());
    resize(settings.value("size", size()).toSize());

    if(settings.value("maximized", true).toBool())
        showMaximized();

    settings.endGroup();
}

void MainWindow::on_actionImport_triggered()
{
    ImportDialog *dialog = new ImportDialog(m_libraryManager, 0);
    connect(dialog, SIGNAL(openBook(int)), this, SLOT(openBook(int)));

    dialog->show();
}

void MainWindow::lastTabClosed()
{
}

void MainWindow::on_actionShamelaImport_triggered()
{
    ShamelaImportDialog importDialog;
    importDialog.setLibraryInfo(m_libraryManager->connectionInfo());

    importDialog.exec();
}

LibraryInfo *MainWindow::libraryInfo()
{
    Q_ASSERT(m_libraryInfo);

    return m_libraryInfo;
}

LibraryManager *MainWindow::libraryManager()
{
    Q_ASSERT(m_libraryInfo);

    return m_libraryManager;
}

BooksViewer *MainWindow::booksViewer()
{
    return m_bookView;
}

BooksListBrowser *MainWindow::booksListBrowser()
{
    return m_booksList;
}

IndexTracker *MainWindow::indexTracker()
{
    Q_ASSERT(m_indexTracker);

    return m_indexTracker;
}

IndexManager * MainWindow::indexManager()
{
    Q_ASSERT(m_indexManager);

    return m_indexManager;
}

BookReaderHelper *MainWindow::readerHelper()
{
    Q_ASSERT(m_readerHelper);

    return m_readerHelper;
}

void MainWindow::controlCenter()
{
    ControlCenterDialog dialog(this);
    dialog.exec();
}

void MainWindow::startIndexing()
{
    m_indexBar->show();
}

void MainWindow::stopIndexing()
{
    m_indexBar->hide();
}

void MainWindow::indexProgress(int value, int max)
{
    m_indexBar->setMaximum(max);
    m_indexBar->setValue(value);
}
