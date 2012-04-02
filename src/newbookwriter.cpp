#include "newbookwriter.h"
#include "bookexception.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "utils.h"
#include "librarybook.h"
#include <stringutils.h>

#include <qsettings.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qdebug.h>

NewBookWriter::NewBookWriter()
{
    m_tempFolder = MW->libraryInfo()->tempDir();
}

NewBookWriter::~NewBookWriter()
{
}

QString NewBookWriter::bookPath()
{
    return m_bookPath;
}

void NewBookWriter::createNewBook(QString bookPath)
{
    if(bookPath.isEmpty())
        m_bookPath = Utils::genBookName(m_tempFolder, true);
    else
        m_bookPath = bookPath;

    while(QFile::exists(m_bookPath)) {
        qWarning() << "createNewBook: File at:" << m_bookPath << "already exists";
        m_bookPath.replace(".alb", "_.alb");
    }

    m_zip.setZipName(m_bookPath);
    if(!m_zip.open(QuaZip::mdCreate)) {
        throw BookException(QObject::tr("لا يمكن انشاء كتاب جديد"), m_bookPath, m_zip.getZipError());
    }
}

int NewBookWriter::addPage(const QString &text, int pageID, int pageNum, int partNum,
                           int hadditNum, int ayaNum, int soraNum)
{
    if(partNum<1)
        partNum = 1;

    if(pageNum<1)
        pageNum = 1;

    QDomElement page = m_pagesDoc.createElement("item");
    page.setAttribute("id", pageID);
    page.setAttribute("page", pageNum);
    page.setAttribute("part", partNum);

    if(hadditNum)
        page.setAttribute("haddit", hadditNum);

    if(soraNum && ayaNum) {
        page.setAttribute("aya", ayaNum);
        page.setAttribute("sora", soraNum);
    }

    QuaZipFile outFile(&m_zip);
    if(outFile.open(QIODevice::WriteOnly,
                     QuaZipNewInfo(QString("pages/p%1.html").arg(pageID)))) {
        QTextStream out(&outFile);
        out << processPageText(text);
    } else {
        qCritical("Can't write to pages/p%d.html - Error: %d", pageID, outFile.getZipError());
    }

    m_pagesElemnent.appendChild(page);

    return pageID;
}

void NewBookWriter::addTitle(const QString &title, int tid, int level)
{
    QDomElement titleElement = m_titlesDoc.createElement("item");
    titleElement.setAttribute("id", ++m_titleID);
    titleElement.setAttribute("pageID", tid);
    titleElement.setAttribute("text", title);

    QDomNode parentNode = m_levels.value(level-1, m_titlesElement);
    m_levels[level] = parentNode.appendChild(titleElement);
}

QString NewBookWriter::processPageText(QString text)
{
    QString htmlText;
    text = Utils::Html::specialCharsEncode(text);

    QRegExp rxMateen(QString::fromUtf8("§([^\"»]+)([»\"])"));
    rxMateen.setMinimal(true);

    QRegExp rxSheer("^([^\\.]+ \\.\\.\\. [^\\.]+)$");
    rxSheer.setMinimal(true);

    QStringList paragraphs = text.split(QRegExp("[\\r\\n]+"));
    foreach(QString p, paragraphs) {
        htmlText.append("<p>");

        htmlText.append(p.simplified()
                        .replace(rxMateen, "<mateen>\\1</mateen>\\2")
                        .replace(rxSheer, "<sheer>\\1</sheer>"));

        htmlText.append("</p>");
    }

    return htmlText;
}

void NewBookWriter::startReading()
{
    m_titleID = 0;

    m_pagesDoc.setContent(QString("<?xml version=\"1.0\" encoding=\"utf-8\" ?><pages></pages>"));
    m_titlesDoc.setContent(QString("<?xml version=\"1.0\" encoding=\"utf-8\" ?><titles></titles>"));

    m_pagesElemnent = m_pagesDoc.documentElement();
    m_titlesElement = m_titlesDoc.documentElement();
    m_lastTitlesElement = m_titlesElement;

    //m_time.start();
}

void NewBookWriter::endReading()
{
    QuaZipFile titlesFile(&m_zip);
    if(titlesFile.open(QIODevice::WriteOnly, QuaZipNewInfo("titles.xml"))) {
        QTextStream out(&titlesFile);
        out.setCodec("utf-8");

        m_titlesDoc.save(out, 1);
    } else {
        qCritical("Can't write to titles.xml - Error: %d", titlesFile.getZipError());
    }

    QuaZipFile pagesFile(&m_zip);
    if(pagesFile.open(QIODevice::WriteOnly, QuaZipNewInfo("pages.xml"))) {
        QTextStream out(&pagesFile);
        out.setCodec("utf-8");

        m_pagesDoc.save(out, 1);
    } else {
        qCritical("Can't write to pages.xml - Error: %d", pagesFile.getZipError());
    }

    m_zip.close();
    // TODO: check if the close success
    //qDebug("[*] Writting take %d ms", m_time.elapsed());
}

