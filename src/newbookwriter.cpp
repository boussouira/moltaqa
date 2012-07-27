#include "newbookwriter.h"
#include "bookexception.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "utils.h"
#include "librarybook.h"
#include "stringutils.h"
#include "xmlutils.h"

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
        m_bookPath = Utils::Rand::fileName(m_tempFolder, true);
    else
        m_bookPath = bookPath;

    while(QFile::exists(m_bookPath)) {
        qWarning() << "NewBookWriter::createNewBook file" << m_bookPath << "already exists";
        m_bookPath.replace(".mlb", "_.mlb");
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
        out.setCodec("utf-8");

        out << processPageText(text);
    } else {
        qCritical("NewBookWriter::addPage error %d when writing to pages/p%d.html",
                  outFile.getZipError(), pageID);
    }

    m_pagesElemnent.appendChild(page);

    return pageID;
}

void NewBookWriter::addTitle(const QString &title, int tid, int level)
{
    QDomElement titleElement = m_titlesDoc.createElement("title");
    titleElement.setAttribute("pageID", tid);

    QDomElement textElement = m_titlesDoc.createElement("text");
    Utils::Xml::setElementText(textElement, m_titlesDoc, title);

    titleElement.appendChild(textElement);

    QDomNode parentNode = m_levels.value(level-1, m_titlesElement);
    m_levels[level] = parentNode.appendChild(titleElement);
}

QString NewBookWriter::processPageText(QString text)
{
    QString htmlText;
    text = Utils::Html::specialCharsEncode(text);

    QRegExp rxMateen(_u("§([^\"»]+)([»\"])"));
    rxMateen.setMinimal(true);

    QRegExp rxSheer("^(.{15,50} [\\.\\*]{3} .{15,50})$");
    rxSheer.setMinimal(true);

    QString specialChar(_u("§"));
    // Separete footnote
    QRegExp footnoteSep("_{6,}");
    QStringList pageTextList = text.split(footnoteSep, QString::SkipEmptyParts);
    if(pageTextList.isEmpty())
        return htmlText;

    if(pageTextList.size() == 2) {
        QString pageText = pageTextList.first();
        QString footnoteText = pageTextList.last().trimmed();

        pageText.replace(QRegExp(_u("\\(¬?([0-9]{1,2})\\)")),
                             "<sup class=\"fnn\"><a class=\"footn\" id=\"fnb\\1\" href=\"#fn\\1\">(\\1)</a></sup>");

        footnoteText.replace(QRegExp(_u("\\(¬?([0-9]{1,2})\\)")),
                             "<sup class=\"fnb\"><a href=\"#fnb\\1\" id=\"fn\\1\">(\\1)</a></sup>");
        footnoteText.replace(QRegExp("[\\r\\n]+"), "<br />");

        footnoteText.prepend("<hr class=\"fns\"/><div class=\"clear\"><footnote>");
        footnoteText.append("</footnote>");

        pageTextList[0] = pageText;
        pageTextList[1] = footnoteText;
    }

    QStringList paragraphs = pageTextList[0].split(QRegExp("[\\r\\n]+"), QString::SkipEmptyParts);
    foreach(QString p, paragraphs) {
        htmlText.append("<p>");

        htmlText.append(p.simplified()
                        .replace(rxMateen, "<mateen>\\1</mateen>\\2")
                        .replace(rxSheer, "<sheer>\\1</sheer>")
                        .remove(specialChar));

        htmlText.append("</p>");
    }

    if(pageTextList.size() == 2)
        htmlText.append(pageTextList[1]);

    return htmlText;
}

void NewBookWriter::startReading()
{
    m_pagesDoc.setContent(QString("<?xml version=\"1.0\" encoding=\"utf-8\" ?><pages></pages>"));
    m_titlesDoc.setContent(QString("<?xml version=\"1.0\" encoding=\"utf-8\" ?><titles></titles>"));

    m_pagesElemnent = m_pagesDoc.documentElement();
    m_titlesElement = m_titlesDoc.documentElement();
    m_lastTitlesElement = m_titlesElement;
}

void NewBookWriter::endReading()
{
    QuaZipFile titlesFile(&m_zip);
    if(titlesFile.open(QIODevice::WriteOnly, QuaZipNewInfo("titles.xml"))) {
        QTextStream out(&titlesFile);
        out.setCodec("utf-8");

        m_titlesDoc.save(out, 1);
    } else {
        qCritical("NewBookWriter::endReading error %d when writing to titles.xml",
                  titlesFile.getZipError());
    }

    QuaZipFile pagesFile(&m_zip);
    if(pagesFile.open(QIODevice::WriteOnly, QuaZipNewInfo("pages.xml"))) {
        QTextStream out(&pagesFile);
        out.setCodec("utf-8");

        m_pagesDoc.save(out, 1);
    } else {
        qCritical("NewBookWriter::endReading error %d when writing to pages.xml",
                  pagesFile.getZipError());
    }

    m_zip.close();

    ml_warn_on_fail(m_zip.getZipError()==0,
                    "NewBookWriter::endReading zip error" << m_zip.getZipError()
                    << "file" << m_zip.getZipName());
}

