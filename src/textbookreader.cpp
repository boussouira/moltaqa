#include "textbookreader.h"
#include "librarybook.h"
#include "bookpage.h"
#include "utils.h"
#include <qxmlstream.h>

TextBookReader::TextBookReader(QObject *parent) :
    AbstractBookReader(parent)
{
    m_loadTitlesText = false;
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

    ml_return_on_fail2(m_zip.setCurrentFile("titles.xml"), "getTitles: setCurrentFile error" << titleFile.getZipError());
    ml_return_on_fail2(titleFile.open(QIODevice::ReadOnly), "getTitles: open error" << titleFile.getZipError());

    QXmlStreamReader reader(&titleFile);
    int titleID = -1;

    while(!reader.atEnd()) {
        reader.readNext();

        if(reader.isStartElement()) {
            if(reader.name() == "title") {
                titleID = reader.attributes().value("pageID").toString().toInt();
                m_titles.append(titleID);
            } else if(m_loadTitlesText && reader.name() == "text") {
                if(reader.readNext() == QXmlStreamReader::Characters) {
                    m_titlesText[titleID] = reader.text().toString();
                } else {
                    qWarning() << "TextBookReader::getTitles Unexpected token type" << reader.tokenString()
                               << "- Book:" << m_bookInfo->id << m_bookInfo->title << m_bookInfo->fileName;
                    break;
                }
            }
        }

        if(reader.hasError()) {
            qDebug() << "getTitles: QXmlStreamReader error:" << reader.errorString()
                     << "- Book:" << m_bookInfo->id << m_bookInfo->title << m_bookInfo->fileName;
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
        ml_return_on_fail2(m_zip.getCurrentFileInfo(&info), "getPages: getCurrentFileInfo Error" << m_zip.getZipError());

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
            qWarning("TextBookReader::getPages zip error %d", m_zip.getZipError());
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
            qWarning("TextBookReader::getPages zip error %d", file.getZipError());
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
