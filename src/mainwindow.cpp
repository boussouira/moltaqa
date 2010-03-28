#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "booksviewer.h"
#include "ksetting.h"
#include <QMessageBox>
#include <QMdiSubWindow>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(trUtf8("برنامج الكتبية"));
    setupActions();

    m_ksetting = new KSetting(this);
    m_bookView = new BooksViewer(this);
    setCentralWidget(m_bookView);
    m_bookView->openSoraInNewTab(1);
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
    m_ksetting->exec();
}
