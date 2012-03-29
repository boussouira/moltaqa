#ifndef ZIPOPENER_H
#define ZIPOPENER_H

class AbstractBookReader;
class ZipHelper;

class ZipOpener {
public:
    ZipOpener(AbstractBookReader *reader);
    ZipOpener(ZipHelper *zip);
    ~ZipOpener();

protected:
    AbstractBookReader *m_reader;
    ZipHelper *m_zip;
};

#endif // ZIPOPENER_H
