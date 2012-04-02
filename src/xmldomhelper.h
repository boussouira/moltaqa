#ifndef XMLDOMHELPER_H
#define XMLDOMHELPER_H

#include <qfile.h>
#include <qdom.h>

class XmlDomHelper
{
public:
    XmlDomHelper();
    ~XmlDomHelper();

    void setFilePath(const QString &path);
    QString filePath();

    void setDocumentName(const QString &name);
    QString documentName();

    void setNeedSave(bool saveDom);
    bool needSave();

    void load();
    void load(QIODevice *file);
    void save();
    void save(const QString &filePath);
    void reload();
    void create();

    QDomElement findElement(const QString &attr, const QString &value);
    QDomElement findElement(const QString &attr, int value) { return findElement(attr, QString::number(value)); }

    QDomElement findElement(const QString &tag, const QString &attr, const QString &value);

    inline QDomElement& rootElement() { return m_rootElement; }
    inline QDomElement& currentElement() { return m_currentElement; }
    inline QDomDocument& domDocument() { return m_doc; }

    void setCurrentElement(QDomElement element) { m_currentElement = element; }

    void setElementText(QDomElement &parent, const QString &tagName, const QString &text, bool cdata=false);

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
