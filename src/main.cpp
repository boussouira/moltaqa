#include "aboutdialog.h"
#include "mainwindow.h"
#include "utils.h"

#ifdef USE_MDBTOOLS
#include "mdbconverter.h"
#endif

#include <qapplication.h>
#include <qdatetime.h>
#include <qdesktopservices.h>
#include <qlocale.h>
#include <qmessagebox.h>
#include <qmutex.h>
#include <qsettings.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qtsingleapplication.h>

#ifdef Q_OS_WIN
#include <Windows.h>

void setArabicKeyboardLayout()
{
    HKL local = LoadKeyboardLayout(L"00000401", KLF_ACTIVATE);

    if(local != NULL)
        ActivateKeyboardLayout(local, KLF_ACTIVATE);
    else
        qWarning("Can't load Arabic Keyboard Layout");
}

#endif

QMutex m_mutex;
QString m_logFilePath;


void debugMessageHandler(QtMsgType type, const char *msg)
{
    QMutexLocker locker(&m_mutex);

    QFile debugFile(m_logFilePath);
    if(!debugFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        fprintf(stderr, "debugMessageHandler: can't open file %s: %s\n",
                qPrintable(m_logFilePath),
                qPrintable(debugFile.errorString()));
        return;
    }

    QTextStream out(&debugFile);
    out.setCodec("utf-8");

    QString dateTime = QDateTime::currentDateTime().toString("[dd/MM/yyyy] [hh:mm:ss] ");
    QString text = QString::fromUtf8(msg);

    if(text.contains('\n'))
        text = text.replace('\n', "\n\t\t\t").trimmed();

    switch (type) {
    case QtDebugMsg:
        out << dateTime << "[DEBUG] " << text << "\n";
        break;
    case QtWarningMsg:
        out << dateTime << "[WARNING] " << text << "\n";
        break;
    case QtCriticalMsg:
        out << dateTime << "[CRITICAL] " << text << "\n";
        break;
    case QtFatalMsg:
        out << dateTime << "[FATAL] " << text << "\n";
        QMessageBox::critical(0, "Fatal error", msg);
        debugFile.close();
        exit(-1);
    }
}

void createLogFileDir()
{
    QDir dir(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
    m_logFilePath = dir.filePath("application.log");

    QFileInfo logInfo(m_logFilePath);
    if(!QFile::exists(logInfo.path())) {
        if(!dir.mkpath(logInfo.path())) {
            return;
        }
    }

    qInstallMsgHandler(debugMessageHandler);
}

void showHelp()
{
    qDebug() << "Usage: moltaqa-lib [ options ... ]";
    qDebug() << "Where options include:";
    qDebug() << "  --version" << "\t" << "Print application version information.";
    qDebug() << "  --debug  " << "\t" << "Print debug messages to standard output.";
    qDebug() << "  --help   " << "\t" << "Show this help message and exit.";
}

int main(int argc, char *argv[])
{
    QtSingleApplication app(argc, argv);
    if(app.arguments().contains("--help")) {
        showHelp();
        return 0;
    }

    if(app.arguments().contains("--version")) {
        AboutDialog::printVersion();
        return 0;
    }

    if (app.sendMessage("Activate!")) {
#ifdef DEV_BUILD
        qWarning("Another instance is already running");
#else
        return 0;
#endif
    }

    app.setLayoutDirection(Qt::RightToLeft);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));

    QSettings::setDefaultFormat(QSettings::IniFormat);

    QTranslator translator;
    translator.load(QString("qt_ar"), App::localeDir());
    app.installTranslator(&translator);

    app.setOrganizationName("Ahl-Alhdeeth");
    app.setOrganizationDomain("ahlalhdeeth.com");
    app.setApplicationName("Moltaqa-Library");
    app.setApplicationVersion("0.5");

    if(!app.arguments().contains("--debug")) {
#ifdef DEV_BUILD
        qWarning() << "Debug messages will be saved in application.log file";
#endif
        createLogFileDir();
    }

    Utils::Rand::srand();

#ifdef Q_OS_WIN
    setArabicKeyboardLayout();
#endif

#ifdef USE_MDBTOOLS
    if(setenv("MDB_JET3_CHARSET", "cp1256", 1))
        qCritical("main: setenv error");

    MdbConverter::init();
#endif

    int ret = -1;

    MainWindow w;
    if(w.init()) {
        w.setLayoutDirection(Qt::RightToLeft);
        w.show();

        QObject::connect(&app, SIGNAL(messageReceived(QString)),
                         &w, SLOT(handleMessage(QString)));

        app.setActivationWindow(&w);

        ret = app.exec();
    }

#ifdef USE_MDBTOOLS
    MdbConverter::exit();
#endif

    return ret;
}
