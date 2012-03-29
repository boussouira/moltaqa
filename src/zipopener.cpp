#include "zipopener.h"
#include "abstractbookreader.h"
#include "ziphelper.h"

ZipOpener::ZipOpener(AbstractBookReader *reader) :
    m_reader(reader),
    m_zip(0)
{
    m_reader->openZip();
}

ZipOpener::ZipOpener(ZipHelper *zip) :
    m_reader(0),
    m_zip(zip)
{
    m_zip->open();
}

ZipOpener::~ZipOpener()
{
    if(m_reader)
        m_reader->closeZip();

    if(m_zip)
        m_zip->close();
}
