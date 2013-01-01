#include "newbookwriter.h"
#include "bookexception.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "utils.h"
#include "librarybook.h"
#include "stringutils.h"
#include "xmlutils.h"
#include "bookutils.h"

#include <qdir.h>
#include <qdatetime.h>
#include <qdebug.h>

NewBookWriter::NewBookWriter()
{
    m_booksDir = MW->libraryInfo()->booksDir();
}

NewBookWriter::~NewBookWriter()
{
    QFile::remove(m_pagesPath);
    QFile::remove(m_titlesPath);
}

QString NewBookWriter::bookPath()
{
    return m_bookPath;
}

void NewBookWriter::createNewBook()
{
    m_bookPath = Utils::Rand::newBook(m_booksDir);

    while(QFile::exists(m_bookPath)) {
        qWarning() << "NewBookWriter::createNewBook file" << m_bookPath << "already exists";
        m_bookPath.replace(".mlb", "_.mlb");
    }

    if(!m_pagesZipWriter.open()) {
        throw BookException("NewBookWriter::createNewBook can't create pages zip file",
                            m_pagesZipWriter.zipPath());
    }

    if(!m_zipWriter.open(m_bookPath)) {
        throw BookException("NewBookWriter::createNewBook can't create zip file",
                            m_zipWriter.zipPath());
    }
}

void NewBookWriter::addPage(BookPage *page)
{
    if(page->part<1)
        page->part = 1;

    if(page->page<1)
        page->page = 1;

    m_pagesWriter.writeStartElement("page");
    m_pagesWriter.writeAttribute("id", QString::number(page->pageID));
    m_pagesWriter.writeAttribute("page", QString::number(page->page));
    m_pagesWriter.writeAttribute("part", QString::number(page->part));

    if(page->haddit)
        m_pagesWriter.writeAttribute("haddit", QString::number(page->haddit));

    if(page->sora && page->aya) {
        m_pagesWriter.writeAttribute("aya", QString::number(page->aya));
        m_pagesWriter.writeAttribute("sora", QString::number(page->sora));
    }

    m_pagesWriter.writeEndElement();

    QString pageText = processPageText(page->text);
    m_pagesZipWriter.add(QString("pages/p%1.html").arg(page->pageID),
                         pageText.toUtf8());
}

void NewBookWriter::addTitle(const QString &title, int tid, int level)
{
    if(m_lastLavel < level) {
        // write element in new level
        ++m_lastLavel;
        m_levels[level] = m_lastLavel;
    } else if(m_lastLavel > level) {
        // iter and close elements
        int toLevel = m_levels[level];
        for(int i=m_lastLavel; i>=toLevel; i--) {
            m_titlesWriter.writeEndElement();
        }

        m_lastLavel = toLevel;
    } else {
        if(m_lastLavel > 0)
            m_titlesWriter.writeEndElement();

    }

    m_titlesWriter.writeStartElement("title");
    m_titlesWriter.writeAttribute("pageID", QString::number(tid));

    m_titlesWriter.writeTextElement("text", title);
}

QString NewBookWriter::processPageText(QString text)
{
    QString htmlText;
    text = text.replace(QRegExp("[\\r\\n]+"), "\n");
    text = Utils::Html::specialCharsEncode(text);

    if(Utils::Book::hasShamelaShoorts(text))
        text = Utils::Book::fixShamelaShoorts(text);

    QRegExp rxMateen(_u("§([^\"»]+)([»\"])"));
    rxMateen.setMinimal(true);

    QRegExp rxSheer("^(.{15,50} [\\.\\*]{3} .{15,50})$");
    rxSheer.setMinimal(true);

    QString specialChar(_u("§"));
    // Separete footnote
    QRegExp footnoteSep("\\n+_{6,}\\n+");
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
        footnoteText.replace("\n", "<br />");

        footnoteText.prepend("<hr class=\"fns\"/><div class=\"clear\"></div><footnote>");
        footnoteText.append("</footnote>");

        pageTextList[0] = pageText;
        pageTextList[1] = footnoteText;
    } else if(pageTextList.size() > 2) {
        pageTextList[0] = text;
    }

    QStringList paragraphs = pageTextList[0].split(QRegExp("\\n+"), QString::SkipEmptyParts);
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
    m_pagesPath = Utils::Rand::fileName(MW->libraryInfo()->tempDir(),
                                        true, "temp_xml_", "xml");

    m_pagesFile.setFileName(m_pagesPath);
    if(!m_pagesFile.open(QIODevice::WriteOnly)) {
        throw BookException("NewBookWriter::startReading open file error",
                            m_pagesFile.errorString());
    }

    m_pagesWriter.setDevice(&m_pagesFile);
    m_pagesWriter.setAutoFormatting(true);

    m_pagesWriter.writeStartDocument();
    m_pagesWriter.writeStartElement("pages");

    // Titles file
    m_titlesPath = Utils::Rand::fileName(MW->libraryInfo()->tempDir(),
                                        true, "temp_xml_", "xml");

    m_titlesFile.setFileName(m_titlesPath);
    if(!m_titlesFile.open(QIODevice::WriteOnly)) {
        throw BookException("NewBookWriter::startReading open file error",
                            m_titlesFile.errorString());
    }

    m_titlesWriter.setDevice(&m_titlesFile);
    m_titlesWriter.setAutoFormatting(true);

    m_titlesWriter.writeStartDocument();
    m_titlesWriter.writeStartElement("titles");

    m_lastLavel = 0;
}

void NewBookWriter::endReading()
{
    if(!m_pagesZipWriter.close())
        throw BookException("NewBookWriter::endReading pages zip file close error");

    // The titles file
    m_titlesWriter.writeEndDocument();
    m_titlesFile.close();

    m_zipWriter.addFromFile("titles.xml", m_titlesPath);

    // Pages info file
    m_pagesWriter.writeEndDocument();
    m_pagesFile.close();

    m_zipWriter.addFromFile("pages.xml", m_pagesPath);

    // Add pages
    m_zipWriter.addFromZip(m_pagesZipWriter.zipPath());

    if(!m_zipWriter.close())
        throw BookException("NewBookWriter::endReading zip file close error");

    m_bookPath = m_zipWriter.zipPath();
}

