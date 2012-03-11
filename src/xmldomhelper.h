#ifndef XMLDOMHELPER_H
#define XMLDOMHELPER_H

#include <qfile.h>
#include <qdom.h>

class XmlDomHelper
{
public:
    XmlDomHelper();
    ~XmlDomHelper();

    void setFilePath(QString &path);
    void setNeedSave(bool saveDom);

    void load();
    void load(QIODevice *file);
    void save();
    void save(const QString &filePath);
    void reload();

    QDomElement findElement(const QString &attr, const QString &value);
    QDomElement findElement(const QString &attr, int value) { return findElement(attr, QString::number(value)); }

    QDomElement findElement(const QString &tag, const QString &attr, const QString &value);

    inline QDomElement& rootElement() { return m_rootElement; }
    inline QDomElement& currentElement() { return m_currentElement; }
    inline QDomDocument& domDocument() { return m_doc; }

    void setCurrentElement(QDomElement element) { m_currentElement = element; }

protected:
    QString m_filePath;
    QDomDocument m_doc;
    QDomElement m_rootElement;
    QDomElement m_currentElement;
    bool m_needSave;
};

#endif // XMLDOMHELPER_H
