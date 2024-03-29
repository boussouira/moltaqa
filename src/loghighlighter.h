#ifndef LOGHIGHLIGHTER_H
#define LOGHIGHLIGHTER_H

#include <qsyntaxhighlighter.h>

class LogHighlighter :  public QSyntaxHighlighter
{
public:
    LogHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);

    void debugHL(const QString &);
    void warningHL(const QString &text);
    void criticalHL(const QString &text);
};

#endif // LOGHIGHLIGHTER_H
