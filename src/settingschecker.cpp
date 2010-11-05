#include "settingschecker.h"
#include "bookinfo.h"
#include <qapplication.h>
#include <qdir.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qdebug.h>

SettingsChecker::SettingsChecker(QObject *parent): QObject(parent)
{
}

void SettingsChecker::checkSettings()
{
    m_settings.beginGroup("General");
    QDir appDir(m_settings.value("app_dir", QApplication::applicationDirPath()).toString());
    QString appDirPath = appDir.absolutePath();
    QString booksFolder = m_settings.value("books_folder", "books").toString();
    QString indexDBName = m_settings.value("index_db", "books_index.db").toString();
    QString booksTemp = m_settings.value("books_temp", "temp").toString();
    m_settings.endGroup();

    if(booksFolder == "books") {
        if(!appDir.exists(booksFolder))
            appDir.mkdir(booksFolder);
    } else {
        QDir booksFolderDir(booksFolder);
        if(!booksFolderDir.exists())
            booksFolderDir.mkpath(booksFolder);
    }
    appDir.cd(booksFolder);

    if(!appDir.exists(indexDBName)) {
        qDebug("Create index database...");
        createIndexBD(appDir.absoluteFilePath(indexDBName));
    }

    if(!appDir.exists(booksTemp))
        appDir.mkdir(booksTemp);

    m_settings.beginGroup("General");
    m_settings.setValue("app_dir", appDirPath);
    m_settings.setValue("books_folder", appDir.absolutePath());
    m_settings.setValue("index_db", indexDBName);
    m_settings.setValue("books_temp", booksTemp);
    m_settings.setValue("index_db_full_path", QString("%1/%2")
                      .arg(appDir.absolutePath())
                      .arg(indexDBName));
    m_settings.endGroup();

    checkDefautQuran();
    checkDefautStyle();
}

void SettingsChecker::checkDefautQuran()
{
    QList<int> quranIDs;
    int quranID = m_settings.value("Books/default_quran", -1).toInt();
    {
        QString indexPath = m_settings.value("General/index_db_full_path").toString();
        QSqlDatabase indexDB = QSqlDatabase::addDatabase("QSQLITE", "INDEX_DB");

        indexDB.setDatabaseName(indexPath);
        if(!indexDB.open())
            return;
        QSqlQuery indexQuery(indexDB);

        indexQuery.exec(QString("SELECT id FROM booksList WHERE bookType = %1")
                         .arg(BookInfo::QuranBook));

        while(indexQuery.next())
            quranIDs.append(indexQuery.value(0).toInt());

        if((quranID == -1 || !quranIDs.contains(quranID)))
            quranID =  (quranIDs.count() > 0) ? quranIDs.first() : -1;

    }
    m_settings.setValue("Books/default_quran", quranID);

    QSqlDatabase::removeDatabase("INDEX_DB");
}

void SettingsChecker::checkDefautStyle()
{
    QString appDir =  m_settings.value("General/app_dir").toString();
    QString styleFolder = m_settings.value("General/styles_dir", "data/css").toString();
    QString currentStyle =  m_settings.value("General/style", "Default").toString();
    QString currentStyleFile = m_settings.value(QString("%1_style/style_file").arg(currentStyle),
                                                "defaut.css").toString();
    QString currentStyleName = m_settings.value(QString("%1_style/style_name").arg(currentStyle),
                                                trUtf8("افتراضي")).toString();
    QString currentStyleFilePath;

    QDir styleDir(QString("%1/%2").arg(appDir).arg(styleFolder));
    if(styleDir.exists()) {
        if(styleDir.exists(currentStyleFile)) {
            currentStyleFilePath = styleDir.absoluteFilePath(currentStyleFile);
        }
    }

    m_settings.setValue("General/styles_dir", styleFolder);
    m_settings.setValue("General/style", currentStyle);

    m_settings.setValue(QString("%1_style/style_name").arg(currentStyle), currentStyleName);
    m_settings.setValue(QString("%1_style/style_file").arg(currentStyle), currentStyleFile);
    m_settings.setValue(QString("%1_style/style_file_path").arg(currentStyle),
                        currentStyleFilePath);
}

void SettingsChecker::createIndexBD(const QString &dbPath)
{
    QString booksListTable = "CREATE TABLE booksList ("
                             "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                             "bookID INTEGER , "
                             "bookType INTEGER , "
                             "bookFlags INTEGER , "
                             "bookCat INTEGER , "
                             "bookName TEXT , "
                             "bookInfo TEXT , "
                             "bookEdition TEXT , "
                             "authorName TEXT , "
                             "authorID INTEGER , "
                             "fileName TEXT , "
                             "bookFolder TEXT, "
                             "pdfID INTEGER )";

    QString catListTable = "CREATE TABLE catList ("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                           "title TEXT , "
                           "description TEXT , "
                           "catOrder INTEGER , "
                           "parentID INTEGER )";
    {
        QSqlDatabase indexDB = QSqlDatabase::addDatabase("QSQLITE", "INDEX_DB");
        indexDB.setDatabaseName(dbPath);
        if(!indexDB.open())
            return;
        QSqlQuery indexQuery(indexDB);

        // Create books list table
        indexQuery.exec(booksListTable);

        // Create categories table and fill it
        if(indexQuery.exec(catListTable))
            fillCatListTable(indexQuery);

    }

    QSqlDatabase::removeDatabase("INDEX_DB");
}

void SettingsChecker::fillCatListTable(QSqlQuery &query)
{
    query.exec(trUtf8("INSERT INTO catList VALUES(1,'العقيدة','',0,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(2,'التفاسير','',1,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(3,'علوم القرآن','',2,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(4,'متون الحديث','',3,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(5,'الأجزاء الحديثية','',4,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(6,'كتب ابن أبي الدنيا','',5,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(7,'شروح الحديث','',6,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(8,'كتب التخريج والزوائد','',7,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(9,'كتب الألباني','',8,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(10,'العلل والسؤالات','',9,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(11,'مصطلح الحديث','',10,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(12,'أصول الفقه والقواعد الفقهية','',11,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(13,'فقه حنفي','',12,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(14,'فقه مالكي','',13,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(15,'فقه شافعي','',14,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(16,'فقه حنبلي','',15,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(17,'فقه عام','',16,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(18,'مسائل فقهية','',17,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(19,'السياسة الشرعية والقضاء','',18,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(20,'الفتاوى','',19,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(21,'كتب ابن تيمية','',20,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(22,'كتب ابن القيم','',21,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(23,'الأخلاق والرقاق والأذكار','',22,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(24,'السيرة والشمائل','',23,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(25,'التاريخ','',24,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(26,'التراجم والطبقات','',25,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(27,'الأنساب','',26,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(28,'البلدان والجغرافيا','',27,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(29,'الغريب والمعاجم ولغة الفقه','',29,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(30,'النحو والصرف','',30,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(31,'الأدب والبلاغة','',31,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(32,'الدواوين الشعرية','',32,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(33,'الجوامع والمجلات ونحوها','',33,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(34,'فهارس الكتب والأدلة','',34,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(35,'محاضرات مفرغة','',35,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(36,'الدعوة وأحوال المسلمين','',36,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(37,'كتب إسلامية عامة','',37,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(38,'علوم أخرى','',38,0)"));
    query.exec(trUtf8("INSERT INTO catList VALUES(39,'كتب اللغة','',28,0)"));
}
