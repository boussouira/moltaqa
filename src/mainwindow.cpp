#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "booksviewer.h"
#include "bookslistbrowser.h"
#include "settingsdialog.h"
#include "bookwidget.h"
#include "settingschecker.h"
#include "importdialog.h"

#include <qmessagebox.h>
#include <qsettings.h>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(trUtf8("برنامج الكتبية"));
    loadSettings();

    m_bookView = new BooksViewer(this);
    m_booksList = new BooksListBrowser(this);
    m_createMenu = true;
    m_bookView->hide();

    setupActions();
}

void MainWindow::setupActions()
{

    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(aboutAlKotobiya()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(settingDialog()));

    connect(ui->pushOpenQuran, SIGNAL(clicked()), this, SLOT(quranWindow()));
    connect(ui->pushBooksList, SIGNAL(clicked()), this, SLOT(showBooksList()));
    connect(m_booksList, SIGNAL(bookSelected(int)), this, SLOT(openBook(int)));
    connect(ui->actionBooksList, SIGNAL(triggered()), this, SLOT(showBooksList()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::aboutAlKotobiya()
{
    QMessageBox::information(this,
                             trUtf8("برنامج الكتبية"),
                             trUtf8("برنامج الكتبية لقراءة القرءان الكريم"));
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
    if(m_createMenu){
        m_bookView->createMenus(this);
        m_createMenu = false;
    }
    if(m_bookView->isHidden())
        m_bookView->show();
    m_bookView->openBook(pBookID);
    setCentralWidget(m_bookView);
}

void MainWindow::showBooksList()
{
    m_booksList->show();
}

void MainWindow::loadSettings()
{
    SettingsChecker checker(this);
    checker.checkSettings();

    QSettings settings;
    defaultQuran = settings.value("Books/default_quran", -1).toInt();
    ui->pushOpenQuran->setEnabled(defaultQuran != -1);
}
