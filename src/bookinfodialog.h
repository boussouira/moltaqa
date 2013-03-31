#ifndef BOOKINFODIALOG_H
#define BOOKINFODIALOG_H

#include <qdialog.h>
#include "librarybook.h"

class WebView;
class QCloseEvent;

class BookInfoDialog : public QWidget
{
public:
    BookInfoDialog(QWidget* parent = 0);

    void setLibraryBook(LibraryBook::Ptr book);
    void setup();

protected:
    void closeEvent(QCloseEvent *event);

protected:
    LibraryBook::Ptr m_book;
    WebView *m_view;
};

#endif // BOOKINFODIALOG_H
