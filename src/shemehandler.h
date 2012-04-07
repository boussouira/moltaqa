#ifndef SHEMEHANDLER_H
#define SHEMEHANDLER_H

#include <qurl.h>

class ShemeHandler
{
public:
    ShemeHandler();

    void open(const QUrl &url);

protected:
    void commandOpen(const QUrl &url);

    void openBook(const QUrl &url);
    void openQuran(const QUrl &url);
    void openAuthor(const QUrl &url);
    void openRawi(const QUrl &url);
};

#endif // SHEMEHANDLER_H
