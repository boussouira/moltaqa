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

#include <qmessagebox.h>
#include <qsettings.h>
#include <qevent.h>
#include <qfile.h>
#include <qtimer.h>
#include <qfiledialog.h>

static MainWindow *m_mainWindow = 0;

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_libraryInfo(0),
    m_libraryManager(0)
{
    ui->setupUi(this);
    m_mainWindow = this;

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
                                        tr("لم يتم العثور على المكتبة في المجلد:" "<br>") + QDir(libDir).absolutePath() + "<br>" +
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

    m_welcomeWidget = new WelcomeWidget(this);
    ui->stackedWidget->addWidget(m_welcomeWidget);
    ui->stackedWidget->setCurrentIndex(0);

    try {
        m_libraryInfo = new LibraryInfo(libDir);

        m_libraryManager = new LibraryManager(m_libraryInfo);
        m_libraryManager->open();
        m_libraryManager->loadBooksListModel();

        m_bookView = new BooksViewer(m_libraryManager, this);
        m_booksList = new BooksListBrowser(m_libraryManager, 0);

        setupActions();

        ui->stackedWidget->addWidget(m_bookView);
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
    delete ui;

    if(m_libraryManager)
        delete m_libraryManager;

    if(m_libraryInfo)
        delete m_libraryInfo;

    m_mainWindow = 0;
}

MainWindow *MainWindow::mainWindow()
{
    return m_mainWindow;
}

void MainWindow::setupActions()
{

    connect(ui->actionExit, SIGNAL(triggered()), SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), SLOT(aboutAlKotobiya()));
    connect(ui->actionSettings, SIGNAL(triggered()), SLOT(settingDialog()));
    connect(ui->actionControlCenter, SIGNAL(triggered()), SLOT(controlCenter()));

    //TODO: open Quran quickly
    connect(m_welcomeWidget, SIGNAL(showBooksList()), SLOT(showBooksList()));
    connect(m_bookView, SIGNAL(lastTabClosed()), SLOT(lastTabClosed()));
    connect(m_booksList, SIGNAL(bookSelected(int)), SLOT(openBook(int)));
    connect(ui->actionBooksList, SIGNAL(triggered()), SLOT(showBooksList()));
}

void MainWindow::aboutAlKotobiya()
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
    m_bookView->showToolBar();

    try {
        m_bookView->openBook(pBookID);

        if(m_bookView->isHidden())
            m_bookView->show();

        ui->stackedWidget->setCurrentIndex(1);

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

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("maximized", isMaximized());
    settings.endGroup();

    m_booksList->close();

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
    ui->stackedWidget->setCurrentIndex(0);
    m_bookView->removeToolBar();
}

void MainWindow::on_actionShamelaImport_triggered()
{
    ShamelaImportDialog importDialog;
    importDialog.setLibraryInfo(m_libraryManager->connectionInfo());

    importDialog.exec();
}

LibraryInfo *MainWindow::libraryInfo()
{
    return m_libraryInfo;
}

LibraryManager *MainWindow::libraryManager()
{
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

void MainWindow::controlCenter()
{
    ControlCenterDialog dialog(this);
    dialog.exec();
}
