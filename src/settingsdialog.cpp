#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "bookreferedialog.h"
#include "clconstants.h"
#include "indexmanager.h"
#include "librarybookmanager.h"
#include "librarymanager.h"
#include "mainwindow.h"
#include "searchmanager.h"
#include "timeutils.h"
#include "utils.h"

#include <qfile.h>
#include <qfiledialog.h>
#include <qfont.h>
#include <qmessagebox.h>
#include <qsettings.h>
#include <qthread.h>
#include <qvariant.h>
#include <qwebsettings.h>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setModal(true);
    loadSettings();

    m_needAppRestart = false;

    QSettings settings;
    ui->tabWidget->setCurrentIndex(settings.value("SettingsDialog/tabIndex", 0).toInt());

    connect(ui->pushBooksDir, SIGNAL(clicked()), this, SLOT(changeBooksDir()));
    connect(ui->pushResetSettings, SIGNAL(clicked()), this, SLOT(resetSettings()));
    connect(ui->buttonBox->button(QDialogButtonBox::Save), SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->pushDeleteSavedSearch, SIGNAL(clicked()), SLOT(deleteSavedSearch()));
    connect(ui->pushOptimizeIndex, SIGNAL(clicked()), SLOT(optimizeIndex()));
    connect(ui->pushUpdateIndex, SIGNAL(clicked()), SLOT(updateIndex()));
    connect(ui->pushClearBooksHistory, SIGNAL(clicked()), SLOT(deleteBooksHistory()));
    connect(ui->pushClearLastOpenedBooks, SIGNAL(clicked()), SLOT(deleteLastOpenedBooks()));

    connect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), SLOT(fontSettingChange()));
    connect(ui->comboFontSize, SIGNAL(currentIndexChanged(int)), SLOT(fontSettingChange()));
}

SettingsDialog::~SettingsDialog()
{
    QSettings settings;
    settings.setValue("SettingsDialog/tabIndex", ui->tabWidget->currentIndex());

    delete ui;
}

void SettingsDialog::loadSettings()
{
    QSettings settings;
    QString booksPath = settings.value("library_dir").toString();
    if(booksPath.size())
        ui->lineBooksDir->setText(booksPath);

    settings.beginGroup("Search");
    ui->spinResultPeerPage->setValue(settings.value("resultPeerPage", 10).toInt());
    ui->spinThreadCount->setValue(settings.value("threadCount", QThread::idealThreadCount()).toInt());
    ui->spinThreadCount->setMinimum(1);
    ui->spinThreadCount->setMaximum(QThread::idealThreadCount()*2);

    ui->comboIndexingRam->addItem(tr("%1 ميغا").arg(100), 100);
    ui->comboIndexingRam->addItem(tr("%1 ميغا").arg(200), 200);
    ui->comboIndexingRam->addItem(tr("%1 ميغا").arg(300), 300);
    ui->comboIndexingRam->addItem(tr("%1 ميغا").arg(500), 500);
    ui->comboIndexingRam->addItem(tr("%1 جيغا").arg(1), 1000);
    ui->comboIndexingRam->addItem(tr("%1 جيغا").arg(1.5), 1500);
    ui->comboIndexingRam->addItem(tr("%1 جيغا").arg(2), 2000);
    ui->comboIndexingRam->addItem(tr("%1 جيغا").arg(3), 3000);

    int currentSize = settings.value("ramSize", DEFAULT_INDEXING_RAM).toInt();
    for(int i=0; i<ui->comboIndexingRam->count(); i++) {
        if(ui->comboIndexingRam->itemData(i).toInt() == currentSize) {
            ui->comboIndexingRam->setCurrentIndex(i);
            break;
        }
    }

    ui->checkSaveSearch->setChecked(settings.value("saveSearch", true).toBool());
    ui->checkShowMessageAfterSearch->setChecked(settings.value("showMessageAfterSearch", false).toBool());
    ui->checkAutoUpdateIndex->setChecked(settings.value("autoUpdateIndex", true).toBool());
    ui->checkHierarchyTitle->setChecked(settings.value("hierarchyTitle", true).toBool());
    ui->checkReturnToSearchWidget->setChecked(settings.value("returnToSearchWidget", true).toBool());
    ui->spinMaxBookToUpdate->setValue(settings.value("maxBookToUpdate", 0).toInt());

    settings.endGroup();

    loadStyles();

    ui->checkAutoUpdate->setChecked(settings.value("Update/autoCheck", true).toBool());
    ui->checkShowCloseWarning->setChecked(settings.value("showCloseWarning", true).toBool());
    ui->checkSaveSearchOptions->setChecked(settings.value("SearchWidget/saveSearchOptions", true).toBool());

    settings.beginGroup("Style");
    QString currentStyle = settings.value("name", ML_DEFAULT_STYLE).toString();
    for(int i=0; i<ui->comboStyles->count(); i++) {
        if(ui->comboStyles->itemData(i).toHash().value("dir").toString() == currentStyle) {
            ui->comboStyles->setCurrentIndex(i);
            break;
        }
    }

    QWebSettings *webSettings = QWebSettings::globalSettings();
    QString fontString = settings.value("fontFamily", webSettings->fontFamily(QWebSettings::StandardFont)).toString();
    int fontSize = settings.value("fontSize", webSettings->fontSize(QWebSettings::DefaultFontSize)).toInt();

    QFont font;
    font.fromString(fontString);

    if(fontSize < 9 || 72 < fontSize)
        fontSize = 9;

    ui->fontComboBox->setCurrentFont(font);
    ui->comboFontSize->setCurrentIndex(ui->comboFontSize->findText(QString::number(fontSize)));

    ui->checkSingleIndexClick->setChecked(settings.value("singleIndexClick", false).toBool());
    ui->checkRemoveTashekil->setChecked(settings.value("removeTashekil", false).toBool());
    ui->checkShowQuranFirst->setChecked(settings.value("showQuranFirst", true).toBool());
    ui->checkDrawAyatNumber->setChecked(settings.value("drawAyatNumber", true).toBool());

    loadSearchFields();
}

void SettingsDialog::loadStyles()
{
    QDir dir(App::stylesDir());
    foreach (QString style, dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot)) {
        QDir styleDir(dir.absoluteFilePath(style));
        if(styleDir.exists("config.cfg")) {
            QSettings cfg(styleDir.absoluteFilePath("config.cfg"), QSettings::IniFormat);
            cfg.setIniCodec("utf-8");

            if(cfg.contains("name") && cfg.contains("description")) {
                QHash<QString, QVariant> styleInfo;
                styleInfo["name"] = cfg.value("name");
                styleInfo["description"] = cfg.value("description");
                styleInfo["dir"] = style;

                ui->comboStyles->addItem(_u(cfg.value("name").toByteArray()), styleInfo);
            }
        }
    }
}

void SettingsDialog::loadSearchFields()
{
    ui->comboSearchFields->clear();
    ui->comboSearchFields->addItem(tr("لا شيء"),
                                   -1);
    ui->comboSearchFields->addItem(tr("كل الكتب"),
                                   -2);

    foreach(SearchFieldInfo info, LibraryManager::instance()->searchManager()->getFieldNames()) {
        ui->comboSearchFields->addItem(info.name, info.fieldID);
    }

    QSettings settings;
    int currentField = settings.value("Search/defaultField", -1).toInt();
    for(int i=0; i<ui->comboSearchFields->count(); i++) {
        if(ui->comboSearchFields->itemData(i).toInt() == currentField) {
            ui->comboSearchFields->setCurrentIndex(i);
            break;
        }
    }
}

void SettingsDialog::resetSettings()
{
    ml_return_on_fail(QMessageBox::question(this,
                                            tr("الخيارات الإفتراضية"),
                                            tr("هل انت متأكد من انك تريد إستعادة الخيارات الإفتراضية للبرنامج؟"),
                                            QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes);

    QSettings settings;

    QStringList keys;
    keys << "showCloseWarning"
         << "Search/resultPeerPage"
         << "Search/threadCount"
         << "Search/ramSize"
         << "Search/saveSearch"
         << "Search/showMessageAfterSearch"
         << "Search/autoUpdateIndex"
         << "Search/hierarchyTitle"
         << "Search/maxBookToUpdate"
         << "Search/defaultField"
         << "Search/returnToSearchWidget"
         << "Update/autoCheck"
         << "Update/lastCheck"
         << "Statistics/last"
         << "SearchWidget/saveSearchOptions"
         << "SearchWidget/sortSearch"
         << "SearchWidget/searchField"
         << "SearchWidget/checkQueryMust"
         << "SearchWidget/checkQueryShould"
         << "SearchWidget/checkQueryShouldNot"
         << "SearchWidget/showPageInfo"
         << "SearchWidget/showResultTitles"
         << "Style/name"
         << "Style/fontFamily"
         << "Style/fontSize"
         << "Style/singleIndexClick"
         << "Style/removeTashekil"
         << "Style/showQuranFirst"
         << "Style/fixShamelaShoorts"
         << "Style/drawAyatNumber"
         << "BookWidget/splitter"
         << "ShamelaImportDialog/threadCount"
         << "WelcomeWidget/tab"
         << "IndexWidget/updateTitle"
         << "AuthorsView/splitter"
         << "BookManagerWidget/splitter"
         << "RowatView/splitter"
         << "BookEditorView/splitter"
         << "RowatManagerWidget/splitter"
         << "WebView/zoom"
         << "BookWidgetManager/splitter"
         << "AuthorsView/last"
         << "AuthorsManagerWidget/splitter"
         << "ExportDialog/exportInOnePackage"
         << "ExportDialog/openOutDit"

            // Groups
         << "CheckableMessageBox"
         << "WidgetStat"
         << "ToolBars"
         << "BooksListWidget"
         << "SearchWidget"
         << "SavedPath"
         << "TreeViewStat";

    foreach (QString key, keys) {
        settings.remove(key);
    }

    m_needAppRestart = true;

    loadSettings();
}

QString SettingsDialog::getFilePath()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("SQLite database (*.db);;All files (*.*)"));
    dialog.setViewMode(QFileDialog::Detail);

    return (dialog.exec()) ? dialog.selectedFiles().first() : QString();
}

QString SettingsDialog::getFolderPath(const QString &defaultPath)
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("اختر مجلد"),
                                                    defaultPath,
                                                    QFileDialog::ShowDirsOnly
                                                    |QFileDialog::DontResolveSymlinks);
    if(dirPath.size()) {
        QDir dir(dirPath);
        return dir.absolutePath();
    }

    return QString();
}

void SettingsDialog::saveSetting(QSettings &settings, const QString &group, const QString &key, const QVariant &value, bool needRestart)
{
    if(group.size())
        settings.beginGroup(group);

    if(needRestart && !m_needAppRestart) {
        if(settings.value(key) != value)
            m_needAppRestart = true;
    }

    settings.setValue(key, value);

    if(group.size())
        settings.endGroup();

}

void SettingsDialog::changeBooksDir()
{
    QString filePath = getFolderPath(ui->lineBooksDir->text());
    if(filePath.size()) {
        ui->lineBooksDir->setText(filePath);
    }
}

void SettingsDialog::saveSettings()
{
    QSettings settings;
    QString appPath = ui->lineBooksDir->text();

    saveSetting(settings, QString(), "library_dir", appPath, true);
    saveSetting(settings, QString(), "showCloseWarning", ui->checkShowCloseWarning->isChecked());

    saveSetting(settings, "Update", "autoCheck", ui->checkAutoUpdate->isChecked());

    saveSetting(settings, "Search", "resultPeerPage", ui->spinResultPeerPage->value());
    saveSetting(settings, "Search", "threadCount", ui->spinThreadCount->value());
    saveSetting(settings, "Search", "ramSize", ui->comboIndexingRam->itemData(ui->comboIndexingRam->currentIndex()));
    saveSetting(settings, "Search", "defaultField", ui->comboSearchFields->itemData(ui->comboSearchFields->currentIndex()));
    saveSetting(settings, "Search", "saveSearch", ui->checkSaveSearch->isChecked());
    saveSetting(settings, "Search", "showMessageAfterSearch", ui->checkShowMessageAfterSearch->isChecked());
    saveSetting(settings, "Search", "autoUpdateIndex", ui->checkAutoUpdateIndex->isChecked());
    saveSetting(settings, "Search", "hierarchyTitle", ui->checkHierarchyTitle->isChecked());
    saveSetting(settings, "Search", "returnToSearchWidget", ui->checkReturnToSearchWidget->isChecked());
    saveSetting(settings, "Search", "maxBookToUpdate", ui->spinMaxBookToUpdate->value());

    saveSetting(settings, "SearchWidget", "saveSearchOptions", ui->checkSaveSearchOptions->isChecked());

    saveSetting(settings, "Style", "name", ui->comboStyles->itemData(ui->comboStyles->currentIndex(),
                                                        Qt::UserRole).toHash().value("dir"), true);
    saveSetting(settings, "Style", "fontFamily", ui->fontComboBox->currentFont().toString());
    saveSetting(settings, "Style", "fontSize", ui->comboFontSize->currentText());

    saveSetting(settings, "Style", "singleIndexClick", ui->checkSingleIndexClick->isChecked(), true);
    saveSetting(settings, "Style", "removeTashekil", ui->checkRemoveTashekil->isChecked(), true);
    saveSetting(settings, "Style", "showQuranFirst", ui->checkShowQuranFirst->isChecked(), true);
    saveSetting(settings, "Style", "drawAyatNumber", ui->checkDrawAyatNumber->isChecked(), true);


    QWebSettings *webSettings = QWebSettings::globalSettings();
    webSettings->setFontFamily(QWebSettings::StandardFont,
                               ui->fontComboBox->currentFont().toString());
    webSettings->setFontSize(QWebSettings::DefaultFontSize,
                             ui->comboFontSize->currentText().toInt());

    webSettings->clearMemoryCaches();

    if(m_needAppRestart) {
        QMessageBox::information(this,
                                 windowTitle(),
                                 tr("يجب إعادة تشغيل البرنامج لتطبيق التغييرات"));
    }

    accept();
}

void SettingsDialog::deleteSavedSearch()
{
    ml_return_on_fail(QMessageBox::question(this,
                                            tr("عبارات البحث"),
                                            tr("هل انت متأكد من انك تريد حذف كل عبارات البحث التي تم حفظها؟"),
                                            QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes);

    LibraryManager::instance()->searchManager()->removeSavedQueries();
}

void SettingsDialog::deleteLastOpenedBooks()
{
    ml_return_on_fail(QMessageBox::question(this,
                                            tr("أحدث الكتب تصفحا"),
                                            tr("هل انت متأكد من انك تريد حذف كل الكتب من قائمة 'الأحدث تصفحا'؟"),
                                            QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes);

    if(LibraryManager::instance()->bookManager()->deleteBookFromLastOpen()) {
        QMessageBox::information(this,
                                 tr("أحدث الكتب تصفحا"),
                                 tr("تم افراغ قائمة قائمة 'الأحدث تصفحا'"));
    } else {
        QMessageBox::warning(this,
                             tr("أحدث الكتب تصفحا"),
                             tr("حدث خطأ أثناء افراغ قائمة قائمة 'الأحدث تصفحا'"));
    }
}

void SettingsDialog::deleteBooksHistory()
{
    ml_return_on_fail(QMessageBox::question(this,
                                            tr("تاريخ تصفح الكتب"),
                                            tr("هل انت متأكد من انك تريد حذف تاريخ تصفح كل الكتب؟"),
                                            QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes);

    if(LibraryManager::instance()->bookManager()->deleteBookHistory()) {
        QMessageBox::information(this,
                                 tr("تاريخ تصفح الكتب"),
                                 tr("تم حذف تاريخ تصفح كل الكتب"));
    } else {
        QMessageBox::warning(this,
                             tr("تاريخ تصفح الكتب"),
                             tr("حدث خطأ أثناء حذف تاريخ تصفح كل الكتب"));
    }
}

void SettingsDialog::optimizeIndex()
{
    ml_return_on_fail(QMessageBox::question(this,
                                            tr("ضغط الفهرس"),
                                            tr("هل انت متأكد من انك تريد تريد ضغط فهرس؟"
                                               "<br>"
                                               "هذه العملية قد تأخذ بعض الوقت وقد يتجمد البرنامج."),
                                            QMessageBox::Yes|QMessageBox::No, QMessageBox::No)==QMessageBox::Yes);

    QTime time;
    time.start();

    if(MW->indexManager()->optimize()) {
        QMessageBox::information(this,
                                 tr("ضغط الفهرس"),
                                 tr("تم ضغط الفهرس خلال %1").arg(Utils::Time::secondsToString(time.elapsed(), true)));
    } else {
        QMessageBox::warning(this,
                             tr("ضغط الفهرس"),
                             tr("حدث خطأ اثناء ضغط الفهرس"));
    }
}

void SettingsDialog::updateIndex()
{
    MW->indexManager()->start();
}

void SettingsDialog::fontSettingChange()
{
    QWebSettings *webSettings = QWebSettings::globalSettings();
    webSettings->setFontFamily(QWebSettings::StandardFont,
                               ui->fontComboBox->currentFont().toString());
    webSettings->setFontSize(QWebSettings::DefaultFontSize,
                             ui->comboFontSize->currentText().toInt());

    webSettings->clearMemoryCaches();
}

void SettingsDialog::on_pushEditRefer_clicked()
{
    BookRefereDialog dialog(this);
    dialog.exec();
}
