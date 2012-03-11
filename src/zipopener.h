#ifndef ZIPOPENER_H
#define ZIPOPENER_H

class AbstractBookReader;

class ZipOpener {
public:
    ZipOpener(AbstractBookReader *reader);
    ~ZipOpener();

    AbstractBookReader *m_reader;
};

#endif // ZIPOPENER_H
