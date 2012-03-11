#include "zipopener.h"
#include "abstractbookreader.h"

ZipOpener::ZipOpener(AbstractBookReader *reader)
: m_reader(reader)
{
    m_reader->openZip();
}

ZipOpener::~ZipOpener()
{
    m_reader->closeZip();
}
