#include "loghighlighter.h"

enum {
    IN_DEBUG = 1,
    IN_WARNING,
    IN_CRITICAL
};

LogHighlighter::LogHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
}

void LogHighlighter::highlightBlock(const QString &text)
{
    int prev = previousBlockState();

    if(text.contains("[DEBUG]")) {
        debugHL(text);
    } else if(text.contains("[WARNING]")) {
        warningHL(text);
    } else if(text.contains("[CRITICAL]") || text.contains("[FATAL]")) {
        criticalHL(text);
    } else {
        if(prev == IN_DEBUG)
            debugHL(text);
        else if(prev == IN_WARNING)
            warningHL(text);
        else if(prev == IN_CRITICAL)
            criticalHL(text);
    }
}

void LogHighlighter::debugHL(const QString &)
{
    setCurrentBlockState(IN_DEBUG);
}

void LogHighlighter::warningHL(const QString &text)
{
    QTextCharFormat  warrFormat;
    warrFormat.setForeground(QColor("#e90000"));

    setFormat(0, text.size(), warrFormat);
    setCurrentBlockState(IN_WARNING);
}

void LogHighlighter::criticalHL(const QString &text)
{
    QFont font;
    font.setBold(true);

    QTextCharFormat  errorFormat;
    errorFormat.setForeground(QColor("#e90000"));
    errorFormat.setFont(font);

    setFormat(0, text.size(), errorFormat);
    setCurrentBlockState(IN_CRITICAL);
}
