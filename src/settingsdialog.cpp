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
#include <qfontcombobox.h>
#include <qmessagebox.h>
#include <qsettings.h>
#include <qthread.h>
#include <qvariant.h>
#include <qwebsettings.h>

FontSelectorDialog::FontSelectorDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("تغيير الخط"));

    m_fontComboBox = new QFontComboBox(this);
    m_sizeComboBox = new QComboBox(this);

    m_sizeComboBox->insertItems(0, QStringList() << "9"
                                << "10" << "11" << "12" << "13" << "14" << "15" << "16" << "17" << "18"
                                << "20" << "22" << "24" << "26" << "28" << "30" << "32" << "34" << "36"
                                << "40" << "44" << "48" << "56" << "64" << "72");

    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Save);
    buttonBox->setCenterButtons(true);

    QHBoxLayout *fontBox = new QHBoxLayout;
    fontBox->addWidget(new QLabel(tr("الخط:"), this));
    fontBox->addWidget(m_fontComboBox);
    fontBox->addStretch();

    QHBoxLayout *sizeBox = new QHBoxLayout;
    sizeBox->addWidget(new QLabel(tr("الحجم:"), this));
    sizeBox->addWidget(m_sizeComboBox);
    sizeBox->addStretch();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(fontBox);
    layout->addLayout(sizeBox);
    layout->addStretch();
    layout->addWidget(buttonBox);

    connect(buttonBox->button(QDialogButtonBox::Save), SIGNAL(clicked()), SLOT(saveFont()));
    connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), SLOT(reject()));
}

void FontSelectorDialog::loadFont(const QString &settingKey)
{
    QSettings settings;
    settings.beginGroup(settingKey);

    QString fontString = settings.value("fontFamily").toString();
    int fontSize = settings.value("fontSize").toInt();

    QFont font;
    font.fromString(fontString);

    if(fontSize < 9 || 72 < fontSize)
        fontSize = 9;

    m_fontComboBox->setCurrentFont(font);
    m_sizeComboBox->setCurrentIndex(m_sizeComboBox->findText(QString::number(fontSize)));

    m_fontSettingKey = settingKey;
}

QString FontSelectorDialog::selectedFontFamily()
{
    return m_fontComboBox->currentFont().family();
}

int FontSelectorDialog::selectedFontSize()
{
    return m_sizeComboBox->currentText().toInt();
}

void FontSelectorDialog::saveFont()
{
    QSettings settings;
    settings.beginGroup(m_fontSettingKey);

    settings.setValue("fontFamily", m_fontComboBox->currentFont().family());
    settings.setValue("fontSize", m_sizeComboBox->currentText());

    accept();
}

/******************/

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

    connect(ui->pushQuranFont, SIGNAL(clicked()), SLOT(changeQuranFont()));
    connect(ui->pushDefaultFont, SIGNAL(clicked()), SLOT(changeDefaultFont()));
}

SettingsDialog::~SettingsDialog()
{
    QSettings settings;
    settings.setValue("SettingsDialog/tabIndex", ui->tabWidget->currentIndex());

    delete ui;
}

void SettingsDialog::checkDefaultFonts()
{
    QSettings settings;

    settings.beginGroup("QuranFont");

    if (!settings.contains("fontFamily"))
        settings.setValue("fontFamily", ML_QURAN_FONT_FAMILY);

    if (!settings.contains("fontSize"))
        settings.setValue("fontSize", ML_QURAN_FONT_SIZE);

    settings.endGroup();

    settings.beginGroup("DefaultFont");

    if (!settings.contains("fontFamily"))
        settings.setValue("fontFamily", ML_DEFAULT_FONT_FAMILY);

    if (!settings.contains("fontSize"))
        settings.setValue("fontSize", ML_DEFAULT_FONT_SIZE);

    settings.endGroup();
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

    ui->checkSingleIndexClick->setChecked(settings.value("singleIndexClick", false).toBool());
    ui->checkRemoveTashekil->setChecked(settings.value("removeTashekil", false).toBool());
    ui->checkShowQuranFirst->setChecked(settings.value("showQuranFirst", true).toBool());
    ui->checkDrawAyatNumber->setChecked(settings.value("drawAyatNumber", true).toBool());

    loadFontSettings();
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

void SettingsDialog::loadFontSettings()
{
    QSettings settings;

    QString fontString = settings.value("QuranFont/fontFamily").toString();
    int fontSize = settings.value("QuranFont/fontSize").toInt();

    ui->pushQuranFont->setText(QString("%1, %2").arg(fontString).arg(fontSize));

    fontString = settings.value("DefaultFont/fontFamily").toString();
    fontSize = settings.value("DefaultFont/fontSize").toInt();

    ui->pushDefaultFont->setText(QString("%1, %2").arg(fontString).arg(fontSize));
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
         << "QuranFont"
         << "DefaultFont"
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

    checkDefaultFonts();
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

    saveSetting(settings, "Style", "singleIndexClick", ui->checkSingleIndexClick->isChecked(), true);
    saveSetting(settings, "Style", "removeTashekil", ui->checkRemoveTashekil->isChecked(), true);
    saveSetting(settings, "Style", "showQuranFirst", ui->checkShowQuranFirst->isChecked(), true);
    saveSetting(settings, "Style", "drawAyatNumber", ui->checkDrawAyatNumber->isChecked(), true);

    QWebSettings::globalSettings()->clearMemoryCaches();

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
    QWebSettings::globalSettings()->clearMemoryCaches();
}

void SettingsDialog::on_pushEditRefer_clicked()
{
    BookRefereDialog dialog(this);
    dialog.exec();
}

void SettingsDialog::changeQuranFont()
{
    FontSelectorDialog dialog(this);
    dialog.loadFont("QuranFont");

    if (dialog.exec() == QDialog::Accepted) {
        ui->pushQuranFont->setText(QString("%1, %2")
                                   .arg(dialog.selectedFontFamily())
                                   .arg(dialog.selectedFontSize()));

        m_needAppRestart = true;
    }
}

void SettingsDialog::changeDefaultFont()
{
    FontSelectorDialog dialog(this);
    dialog.loadFont("DefaultFont");

    if (dialog.exec() == QDialog::Accepted) {
        ui->pushDefaultFont->setText(QString("%1, %2")
                                     .arg(dialog.selectedFontFamily())
                                     .arg(dialog.selectedFontSize()));

        m_needAppRestart = true;
    }
}
