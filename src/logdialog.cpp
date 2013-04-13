#include "logdialog.h"
#include "ui_logdialog.h"
#include "loghighlighter.h"
#include "utils.h"

#include "checkablemessagebox.h"

#include <qdesktopservices.h>
#include <qdir.h>
#include <qevent.h>
#include <qfile.h>
#include <qfilesystemwatcher.h>
#include <qmessagebox.h>
#include <qtextstream.h>

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
    int rep = CheckableMessageBox::question(this,
                                            windowTitle(),
                                            tr("نافذة الأخطاء تحتوي على معلومات قد تساعد في تصحيح أخطاء البرنامج" "\n"
                                               "هل تريد مسح الأخطاء الموجودة في هذه النافذة؟"),
                                            "CheckableMessageBox/LogDialogClear",
                                            QDialogButtonBox::Yes);
    if(rep == QDialogButtonBox::Yes) {
        QFile log(m_logPath);
        ml_warn_on_fail(log.remove(),
                        "LogDialog::clearLog Error when deleting log file"
                        << log.errorString());

        // Create an empty log file
        ml_warn_on_fail(log.open(QFile::WriteOnly | QFile::Text | QFile::Truncate),
                        "LogDialog::clearLog Error when creating empty log file"
                        << log.errorString());
    }
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
