#include "textbookreader.h"
#include "librarybook.h"
#include "bookpage.h"
#include <qxmlstream.h>

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
    ZipOpener opener(this);

    getTitles();
    getPages();
}

void TextBookReader::getTitles()
{
    QuaZipFile titleFile(&m_zip);

    if(m_zip.setCurrentFile("titles.xml")) {
        if(!titleFile.open(QIODevice::ReadOnly)) {
            qWarning("getTitles: open error %d", titleFile.getZipError());
            return;
        }
    } else {
        qDebug("ERRR");
    }

    QXmlStreamReader reader(&titleFile);
    while(!reader.atEnd()) {
        reader.readNext();

        if(reader.isStartElement() && reader.name() == "item") {
            m_titles.append(reader.attributes().value("pageID").toString().toInt());
        }

        if(reader.hasError()) {
            qDebug() << "getTitles: QXmlStreamReader error:" << reader.errorString();
            break;
        }
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

        QByteArray out;
        char buf[4096];
        int len = 0;

        while (!file.atEnd()) {
            len = file.read(buf, 4096);
            out.append(buf, len);

            if(!len)
                break;
        }

        m_pages.insert(id, out);

        file.close();
        if(file.getZipError()!=UNZ_OK) {
            qWarning("getPages: open error %d", file.getZipError());
            continue;
        }
    }
}

int TextBookReader::getPageTitleID(int pageID)
{
    if(!m_titles.contains(pageID)) {
        int title = 0;
        for(int i=0; i<m_titles.size(); i++) {
            title = m_titles.at(i);
            if(i == m_titles.size()-1)
                return m_titles.last();
            else if(title <= pageID && pageID < m_titles.at(i+1))
                return title;
            if(title > pageID)
                break;
        }

        return m_titles.first();
    }

    return pageID;
}
