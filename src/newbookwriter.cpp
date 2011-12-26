#include "newbookwriter.h"
#include "bookexception.h"
#include "mainwindow.h"
#include "libraryinfo.h"
#include "utils.h"
#include "librarybook.h"

#include <qsettings.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qdebug.h>

NewBookWriter::NewBookWriter()
{
    m_tempFolder = MW->libraryInfo()->tempDir();
    m_pageId = 0;
    m_threadID = 0;
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

    if(QFile::exists(m_bookPath)) {
        qWarning() << "createNewBook: File at:" << m_bookPath << "already exists";
        if(QFile::remove(m_bookPath))
            qWarning() << "Delete it";
        else
            qWarning("Can't delete it!");
    }

    m_file.setFileName(m_bookPath);
    m_zip.setIoDevice(&m_file);
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

    m_pageId++; // Last inserted id

    QDomElement page = m_pagesDoc.createElement("item");
    page.setAttribute("id", m_pageId);
    page.setAttribute("page", pageNum);
    page.setAttribute("part", partNum);

    if(hadditNum != -1)
        page.setAttribute("haddit", hadditNum);

    if(soraNum != -1 && ayaNum != -1) {
        page.setAttribute("aya", ayaNum);
        page.setAttribute("sora", soraNum);
    }

    QuaZipFile outFile(&m_zip);
    if(outFile.open(QIODevice::WriteOnly,
                     QuaZipNewInfo(QString("pages/p%1.html").arg(m_pageId)))) {
        QTextStream out(&outFile);
        out << text;
    } else {
        qCritical("Can't write to pages/p%d.html - Error: %d", m_pageId, outFile.getZipError());
    }

    m_pagesElemnent.appendChild(page);
    m_idsHash.insert(pageID, m_pageId);

    return m_pageId;
}

void NewBookWriter::addTitle(const QString &title, int tid, int level)
{
    int id = m_idsHash.value(tid, -1);
    if(id == -1)
        id = m_prevID;
    else
        m_prevID = id;

    m_titleID++;

    QDomElement titleElement = m_titlesDoc.createElement("item");
    titleElement.setAttribute("id", m_titleID);
    titleElement.setAttribute("pageID", id);
    titleElement.setAttribute("text", title);

    if(level != m_lastLevel) {
        if(level > m_lastLevel) { //Up
            if(level - m_lastLevel != 1)
                qDebug("WARNING: level up by %d at id %d", level-m_lastLevel, tid);

            if(!m_lastTitlesElement.isNull()) {
                QDomNode node = m_lastTitlesElement.appendChild(titleElement);
                if(!node.isNull())
                    m_lastTitlesElement = node.toElement();
            }
        } else { //Down
            int levelCount = m_lastLevel - level;
            while(levelCount>0) {
                QDomNode node = m_lastTitlesElement.parentNode();
                if(!node.isNull())
                    m_lastTitlesElement = node.toElement();

                levelCount--;
            }

            QDomNode parentNode = m_lastTitlesElement.parentNode();
            if(!parentNode.isNull()) {
                QDomNode node = parentNode.appendChild(titleElement);
                if(!node.isNull())
                    m_lastTitlesElement = node.toElement();
            }
        }
    } else {
        QDomNode parentNode = (m_titleID == 1) ? m_lastTitlesElement : m_lastTitlesElement.parentNode();
        if(!parentNode.isNull()) {
            QDomNode node = parentNode.appendChild(titleElement);
            if(!node.isNull())
                m_lastTitlesElement = node.toElement();
        }
    }

    m_lastLevel = level;
}

void NewBookWriter::startReading()
{
    m_pageId = 0;
    m_prevID = 1;
    m_lastLevel = 1;
    m_titleID = 0;

    m_pagesDoc.setContent(QString("<?xml version='1.0' encoding='UTF-8'?><pages></pages>"));
    m_titlesDoc.setContent(QString("<?xml version='1.0' encoding='UTF-8'?><titles></titles>"));

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
        out << m_titlesDoc.toString(-1);
    } else {
        qCritical("Can't write to titles.xml - Error: %d", titlesFile.getZipError());
    }

    QuaZipFile pagesFile(&m_zip);
    if(pagesFile.open(QIODevice::WriteOnly, QuaZipNewInfo("pages.xml"))) {
        QTextStream out(&pagesFile);
        out << m_pagesDoc.toString(-1);
    } else {
        qCritical("Can't write to pages.xml - Error: %d", pagesFile.getZipError());
    }

    m_zip.close();
    // TODO: check if the close success
    //qDebug("[*] Writting take %d ms", m_time.elapsed());
}

