#include "logdialog.h"
#include "ui_logdialog.h"
#include "loghighlighter.h"
#include "utils.h"

#include <qdesktopservices.h>
#include <qfilesystemwatcher.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qevent.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qdebug.h>

LogDialog::LogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialog)
{
    ui->setupUi(this);

    m_logPath = QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)).filePath("application.log");
    m_watcher = new QFileSystemWatcher(this);
    m_highlighter = new LogHighlighter(ui->textBrowser->document());

    connect(m_watcher, SIGNAL(fileChanged(QString)), SLOT(fileChanged(QString)));
    connect(ui->pushClose, SIGNAL(clicked()), SLOT(hideDialog()));
    connect(ui->pushClearLog, SIGNAL(clicked()), SLOT(clearLog()));

    fileChanged(m_logPath);

    Utils::Widget::restore(this, "LogDialog");
}

LogDialog::~LogDialog()
{
    delete ui;
    delete m_watcher;
    delete m_highlighter;
}

void LogDialog::fileChanged(const QString &path)
{
    QFile log(path);
    if(log.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream in(&log);
        in.setCodec("utf-8");
        ui->textBrowser->clear();

        QTextCursor cursor(ui->textBrowser->document());
        cursor.movePosition(QTextCursor::End);
        cursor.movePosition(QTextCursor::Up);
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.insertText(in.readAll());

        ui->textBrowser->ensureCursorVisible();
    }
}

void LogDialog::clearLog()
{
    ml_return_on_fail(QMessageBox::question(this,
                                            windowTitle(),
                                            tr("نافذة الأخطاء تحتوي على معلومات قد تساعد في تصحيح أخطاء البرنامج" "\n"
                                               "هل تريد مسح الأخطاء الموجودة في هذه النافذة؟"),
                                            QMessageBox::Yes|QMessageBox::No,
                                            QMessageBox::No) == QMessageBox::Yes);

    QFile log(m_logPath);
    log.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
}

void LogDialog::closeEvent(QCloseEvent *event)
{
    Utils::Widget::save(this, "LogDialog");

    hideDialog();
    event->ignore();
}

void LogDialog::stopWatching()
{
    m_watcher->removePath(m_logPath);
}

void LogDialog::startWatching()
{
    m_watcher->addPath(m_logPath);
    fileChanged(m_logPath);
}

void LogDialog::hideDialog()
{
    stopWatching();
    hide();
}
