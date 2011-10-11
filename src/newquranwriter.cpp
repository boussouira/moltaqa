#include "newquranwriter.h"
#include "utils.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include <qsettings.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <librarybook.h>

newQuranWriter::newQuranWriter()
{
    m_tempFolder = MainWindow::mainWindow()->libraryInfo()->tempDir();
    m_threadID = 0;
}

void newQuranWriter::createNewBook(QString bookPath)
{
    // TODO: check if this file exsists
    if(bookPath.isEmpty())
        m_bookPath = QString("%1/book_%2.sqlite").arg(m_tempFolder).arg(QDateTime::currentDateTime().toMSecsSinceEpoch());
    else
        m_bookPath = bookPath;

    m_bookDB = QSqlDatabase::addDatabase("QSQLITE", QString("newQuranDB_%1").arg(m_threadID));
    m_bookDB.setDatabaseName(m_bookPath);
    if(!m_bookDB.open()) {
        LOG_DB_ERROR(m_bookDB);
        return;
    }

    m_bookQuery = QSqlQuery(m_bookDB);

    createBookTables();
}

void newQuranWriter::createBookTables()
{
    // TODO: the id columun should be AUTO INCEREMENT?
    m_bookQuery.exec("DROP TABLE IF EXISTS quranSowar");
    m_bookQuery.exec("CREATE TABLE quranSowar ("
                     "id INTEGER PRIMARY KEY,"
                     "SoraName TEXT,"
                     "ayatCount INTEGER,"
                     "SoraDescent TEXT)");

    // TODO: categorie order
    m_bookQuery.exec("DROP TABLE IF EXISTS quranText");
    m_bookQuery.exec("CREATE TABLE quranText ("
                     "id INTEGER PRIMARY KEY, "
                     "ayaText TEXT, "
                     "soraNumber INTEGER, "
                     "ayaNumber INTEGER, "
                     "pageNumber INTEGER)");
}

QString newQuranWriter::bookPath()
{
    return m_bookPath;
}

void newQuranWriter::startReading()
{
    m_bookDB.transaction();
}

void newQuranWriter::endReading()
{
    m_bookDB.commit();
}

void newQuranWriter::addPage(const QString &text, int soraNum, int ayaNum, int pageNum)
{
    m_bookQuery.prepare("INSERT INTO quranText (id, ayaText, soraNumber, ayaNumber, pageNumber) "
                        "VALUES (NULL, ?, ?, ?, ?)");
    m_bookQuery.bindValue(0, text);
    m_bookQuery.bindValue(1, soraNum);
    m_bookQuery.bindValue(2, ayaNum);
    m_bookQuery.bindValue(3, pageNum);

    if(!m_bookQuery.exec())
        LOG_SQL_ERROR(m_bookQuery);
}

void newQuranWriter::addSoraInfo(const QString &name, const QString &SoraDescent, int ayatCount)
{
    m_bookQuery.prepare("INSERT INTO quranSowar (id, SoraName, ayatCount, SoraDescent) "
                        "VALUES (NULL, ?, ?, ?)");
    m_bookQuery.bindValue(0, name);
    m_bookQuery.bindValue(1, ayatCount);
    m_bookQuery.bindValue(2, SoraDescent);

    if(!m_bookQuery.exec())
        LOG_SQL_ERROR(m_bookQuery);
}

void newQuranWriter::addSowarInfo()
{
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(1,'الفاتحة',7,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(2,'البقرة',286,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(3,'آل عمران',200,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(4,'النساء',176,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(5,'المائدة',120,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(6,'الأنعام',165,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(7,'الأعراف',206,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(8,'الأنفال',75,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(9,'التوبة',129,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(10,'يونس',109,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(11,'هود',123,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(12,'يوسف',111,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(13,'الرعد',43,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(14,'إبراهيم',52,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(15,'الحجر',99,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(16,'النحل',128,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(17,'الإسراء',111,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(18,'الكهف',110,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(19,'مريم',98,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(20,'طه',135,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(21,'الأنبياء',112,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(22,'الحج',78,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(23,'المؤمنون',118,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(24,'النور',64,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(25,'الفرقان',77,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(26,'الشعراء',227,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(27,'النمل',93,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(28,'القصص',88,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(29,'العنكبوت',69,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(30,'الروم',60,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(31,'لقمان',34,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(32,'السجدة',30,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(33,'الأحزاب',73,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(34,'سبأ',54,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(35,'فاطر',45,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(36,'يس',83,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(37,'الصافات',182,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(38,'ص',88,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(39,'الزمر',75,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(40,'غافر',85,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(41,'فصلت',54,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(42,'الشورى',53,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(43,'الزخرف',89,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(44,'الدخان',59,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(45,'الجاثية',37,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(46,'الأحقاف',35,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(47,'محمد',38,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(48,'الفتح',29,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(49,'الحجرات',18,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(50,'ق',45,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(51,'الذاريات',60,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(52,'الطور',49,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(53,'النجم',62,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(54,'القمر',55,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(55,'الرحمن',78,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(56,'الواقعة',96,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(57,'الحديد',29,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(58,'المجادلة',22,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(59,'الحشر',24,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(60,'الممتحنة',13,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(61,'الصف',14,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(62,'الجمعة',11,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(63,'المنافقون',11,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(64,'التغابن',18,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(65,'الطلاق',12,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(66,'التحريم',12,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(67,'الملك',30,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(68,'القلم',52,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(69,'الحاقة',52,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(70,'المعارج',44,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(71,'نوح',28,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(72,'الجن',28,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(73,'المزمل',20,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(74,'المدثر',56,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(75,'القيامة',40,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(76,'الإنسان',31,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(77,'المرسلات',50,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(78,'النبأ',40,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(79,'النازعات',46,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(80,'عبس',42,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(81,'التكوير',29,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(82,'الإنفطار',19,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(83,'المطففين',36,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(84,'الإنشقاق',25,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(85,'البروج',22,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(86,'الطارق',17,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(87,'الأعلى',19,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(88,'الغاشية',26,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(89,'الفجر',30,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(90,'البلد',20,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(91,'الشمس',15,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(92,'الليل',21,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(93,'الضحى',11,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(94,'الشرح',8,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(95,'التين',8,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(96,'العلق',19,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(97,'القدر',5,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(98,'البينة',8,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(99,'الزلزلة',8,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(100,'العاديات',11,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(101,'القارعة',11,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(102,'التكاثر',8,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(103,'العصر',3,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(104,'الهمزة',9,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(105,'الفيل',5,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(106,'قريش',4,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(107,'الماعون',7,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(108,'الكوثر',3,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(109,'الكافرون',6,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(110,'النصر',3,'مدني')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(111,'المسد',5,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(112,'الإخلاص',4,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(113,'الفلق',5,'مكي')"));
    m_bookQuery.exec(tr("INSERT INTO quranSowar VALUES(114,'الناس',6,'مكي')"));
}
