#include "textbookreader.h"
#include "librarybook.h"
#include "bookpage.h"

TextBookReader::TextBookReader(QObject *parent) :
    AbstractBookReader(parent)
{
}

TextBookReader::~TextBookReader()
{
    m_titles.clear();
    m_pages.clear();
}

void TextBookReader::firstPage()
{
    AbstractBookReader::firstPage();

    m_currentPage->titleID = m_titles.first();
}

void TextBookReader::load()
{
    //QTime t;

    //t.start();
    getTitles();
    //qDebug("Load Titles take %d ms", t.elapsed());

    //t.restart();
    getPages();
    //qDebug("Load Pages take %d ms", t.elapsed());
}

void TextBookReader::getTitles()
{
    QuaZipFile titleFile(&m_zip);

    if(m_zip.setCurrentFile("titles.xml")) {
        if(!titleFile.open(QIODevice::ReadOnly)) {
            qWarning("testRead(): file.open(): %d", titleFile.getZipError());
            return;
        }
    }

    QDomDocument doc;
    QString errorStr;
    int errorLine=0;
    int errorColumn=0;

    if(!doc.setContent(&titleFile, 0, &errorStr, &errorLine, &errorColumn)) {
        qDebug("getTitles: Parse error at line %d, column %d: %s\nFile: %s",
               errorLine, errorColumn,
               qPrintable(errorStr),
               qPrintable(m_bookInfo->bookPath));

        titleFile.close();
        return;
    }

    QDomElement root = doc.documentElement();
    QDomNode itemNode = root.firstChild();

    while(!itemNode.isNull()) {
        m_titles.append(itemNode.attributes().namedItem("pageID").nodeValue().toInt());

        itemNode = itemNode.nextSibling();
    }

    qSort(m_titles);

    titleFile.close();
}

void TextBookReader::getPages()
{
    QuaZipFileInfo info;
    QuaZipFile file(&m_zip);
    for(bool more=m_zip.goToFirstFile(); more; more=m_zip.goToNextFile()) {
        if(!m_zip.getCurrentFileInfo(&info)) {
            qWarning("getPages: getCurrentFileInfo Error %d", m_zip.getZipError());
            return;
        }

        int id = 0;
        QString name = info.name;
        if(name.startsWith("pages/p")) {
            name = name.remove(0, 7);
            name = name.remove(".html");

            bool ok;
            id = name.toInt(&ok);
            if(!ok) {
                qDebug("can't convert '%s' to int", qPrintable(name));
                continue;
            }
        } else {
            continue;
        }

        if(!file.open(QIODevice::ReadOnly)) {
            qWarning("getPages: open Error %d", m_zip.getZipError());
            continue;
        }

        m_pages.insert(id, QString::fromUtf8(file.readAll()));

        file.close();
        if(file.getZipError()!=UNZ_OK) {
            qWarning("getPages: open error %d", file.getZipError());
            continue;
        }
    }
}

void TextBookReader::connected()
{
    AbstractBookReader::connected();
}
