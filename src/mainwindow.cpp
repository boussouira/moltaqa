#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "booksviewer.h"
#include "booksbrowser.h"
#include "ksetting.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(trUtf8("برنامج الكتبية"));
    setupActions();

    connect(ui->pushOpenQuran, SIGNAL(clicked()), this, SLOT(quranWindow()));
    connect(ui->pushBooksList, SIGNAL(clicked()), this, SLOT(showBooksList()));
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
    m_bookView = new BooksViewer(this);
    setCentralWidget(m_bookView);
    m_bookView->openSoraInNewTab(1);
}

void MainWindow::showBooksList()
{
    BooksBrowser *booksList = new BooksBrowser(this);
    booksList->show();
}
