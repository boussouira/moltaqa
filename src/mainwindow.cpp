#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "booksviewer.h"
#include "indexdb.h"
#include "bookslistbrowser.h"
#include "settingsdialog.h"
#include "bookwidget.h"
#include "settingschecker.h"
#include "importdialog.h"
#include "bookexception.h"
#include "libraryinfo.h"
#include "welcomewidget.h"
#include "shamelaimportdialog.h"

#include <qmessagebox.h>
#include <qsettings.h>
#include <qevent.h>

static MainWindow *m_mainWindow = 0;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("برنامج الكتبية"));
    loadSettings();

    m_mainWindow = this;

    m_welcomeWidget = new WelcomeWidget(this);
    ui->stackedWidget->addWidget(m_welcomeWidget);
    ui->stackedWidget->setCurrentIndex(0);

    try {
        /* Temporary code */
        QSettings settings;
        QString libDir = settings.value("library_dir").toString();

        m_libraryInfo = new LibraryInfo(libDir);
        /* Temporary code */

        m_indexDB = new IndexDB(m_libraryInfo);
        m_indexDB->open();

        m_bookView = new BooksViewer(m_indexDB, this);
        m_booksList = new BooksListBrowser(m_indexDB, 0);

        setupActions();

        ui->stackedWidget->addWidget(m_bookView);
    } catch(BookException &e) {
        QMessageBox::information(this,
                                 tr("برنامج الكتبية"),
                                 tr("حدث خطأ أثناء تحميل البرنامج:"
                                    "<br>%1").arg(e.what()));
        setEnabled(false);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_indexDB;
    delete m_libraryInfo;
    m_mainWindow = 0;
}

MainWindow *MainWindow::mainWindow()
{
    return m_mainWindow;
}

void MainWindow::setupActions()
{

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(aboutAlKotobiya()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(settingDialog()));

    //TODO: open Quran quickly
    connect(m_welcomeWidget, SIGNAL(showBooksList()), SLOT(showBooksList()));
    connect(m_bookView, SIGNAL(lastTabClosed()), SLOT(lastTabClosed()));
    connect(m_booksList, SIGNAL(bookSelected(int)), this, SLOT(openBook(int)));
    connect(ui->actionBooksList, SIGNAL(triggered()), this, SLOT(showBooksList()));
}

void MainWindow::aboutAlKotobiya()
{
    QMessageBox::information(this,
                             tr("برنامج الكتبية"),
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
                                 tr("برنامج الكتبية"),
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
    ImportDialog *dialog = new ImportDialog(m_indexDB, 0);

    connect(dialog, SIGNAL(openBook(int)), this, SLOT(openBook(int)));
    connect(dialog, SIGNAL(bookAdded()), m_booksList, SLOT(loadBooksList()));

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
    importDialog.setLibraryInfo(m_indexDB->connectionInfo());

    importDialog.exec();
}

LibraryInfo *MainWindow::libraryInfo()
{
    return m_libraryInfo;
}

IndexDB *MainWindow::indexDB()
{
    return m_indexDB;
}

BooksViewer *MainWindow::booksViewer()
{
    return m_bookView;
}

BooksListBrowser *MainWindow::booksListBrowser()
{
    return m_booksList;
}
