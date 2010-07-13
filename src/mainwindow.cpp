#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "booksviewer.h"
#include "bookslistbrowser.h"
#include "ksetting.h"
#include "bookwidget.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(trUtf8("برنامج الكتبية"));
    setupActions();

    m_bookView = new BooksViewer(this);
    m_booksList = new BooksListBrowser(this);
    m_createMenu = true;

    connect(ui->pushOpenQuran, SIGNAL(clicked()), this, SLOT(quranWindow()));
    connect(ui->pushBooksList, SIGNAL(clicked()), this, SLOT(showBooksList()));
    connect(m_booksList, SIGNAL(bookSelected(int)), this, SLOT(openBook(int)));
}

void MainWindow::setupActions()
{

    connect(ui->actionExit, SIGNAL(triggered()),
            this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()),
            this, SLOT(aboutAlKotobiya()));
    connect(ui->actionSettings, SIGNAL(triggered()),
            this, SLOT(settingDialog()));
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
    KSetting *ksetting = new KSetting(this);
    ksetting->exec();
}

void MainWindow::quranWindow()
{
    openBook(2);
}

void MainWindow::openBook(int pBookID)
{
    m_bookView->openBook(pBookID);
    setCentralWidget(m_bookView);
    if(m_createMenu){
        m_bookView->createMenus(this);
        m_createMenu = false;
    }
}

void MainWindow::showBooksList()
{
    m_booksList->show();
}
