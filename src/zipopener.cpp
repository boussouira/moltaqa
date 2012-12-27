#include "zipopener.h"
#include "abstractbookreader.h"

ZipOpener::ZipOpener(AbstractBookReader *reader) :
    m_reader(reader)
{
    m_reader->openZip();
}


ZipOpener::~ZipOpener()
{
    if(m_reader)
        m_reader->closeZip();
}
