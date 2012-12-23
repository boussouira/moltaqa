#ifndef XMLDOMHELPER_H
#define XMLDOMHELPER_H

#include <qfile.h>
#include <qdom.h>
#include <qsharedpointer.h>

class XmlDomHelper
{
public:
    XmlDomHelper();
    ~XmlDomHelper();

    typedef QSharedPointer<XmlDomHelper> Ptr;

    void setFilePath(const QString &path);
    QString filePath();

    void setDocumentName(const QString &name);
    QString documentName();

    void setNeedSave(bool saveDom);
    bool needSave();

    void load();
    void load(QIODevice *file);
    void maySave();
    void save();
    void save(const QString &filePath);
    void save(QIODevice *ioDevice);
    void reload();
    void mayCreate();
    void create();

    QDomElement findElement(const QString &attr, const QString &value) { return findElement(QString(), attr, value); }
    QDomElement findElement(const QString &attr, int value) { return findElement(QString(), attr, QString::number(value)); }
    QDomElement findElement(const QString &tag, const QString &attr, const QString &value);

    QDomElement treeFindElement(const QString &attr, const QString &value) { return treeFindElement(QString(), attr, value);}
    QDomElement treeFindElement(const QString &attr, int value) { return treeFindElement(QString(), attr, QString::number(value));}
    QDomElement treeFindElement(const QString &tag, const QString &attr, const QString &value);

    QList<QDomElement> treeFindElements(const QString &attr, const QString &value) { return treeFindElements(QString(), attr, value);}
    QList<QDomElement> treeFindElements(const QString &attr, int value) { return treeFindElements(QString(), attr, QString::number(value));}
    QList<QDomElement> treeFindElements(const QString &tag, const QString &attr, int value) { return treeFindElements(tag, attr, QString::number(value));}
    QList<QDomElement> treeFindElements(const QString &tag, const QString &attr, const QString &value);

    inline QDomElement& rootElement() { return m_rootElement; }
    inline QDomElement& currentElement() { return m_currentElement; }
    inline QDomDocument& domDocument() { return m_doc; }

    void setCurrentElement(QDomElement element) { m_currentElement = element; }

    QDomElement setElementText(QDomElement &parent, const QString &tagName, const QString &text, bool cdata=false);

protected:
    QString m_filePath;
    QString m_documentName;
    QDomDocument m_doc;
    QDomElement m_rootElement;
    QDomElement m_currentElement;
    bool m_needSave;
    bool m_domLoaded;
};

#endif // XMLDOMHELPER_H
