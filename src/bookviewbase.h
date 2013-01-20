#ifndef BOOKVIEWBASE_H
#define BOOKVIEWBASE_H

#include <qwidget.h>
#include "librarybook.h"

class AbstractBookReader;
class WebViewSearcher;

class BookViewBase : public QWidget
{
    Q_OBJECT
public:
    BookViewBase(LibraryBook::Ptr book, QWidget *parent = 0);
    
    LibraryBook::Ptr book();

    virtual AbstractBookReader *bookReader()=0;
    virtual WebViewSearcher *viewSearcher()=0;

    virtual void hideIndexWidget();

    virtual void saveSettings();
    virtual void loadSettings();

public slots:
    virtual void firstPage()=0;
    virtual void lastPage()=0;
    virtual void nextPage()=0;
    virtual void prevPage()=0;

    virtual void openPage(int id)=0;
    virtual void openPage(int pageNum, int partNum)=0;
    virtual void openSora(int sora, int aya)=0;
    virtual void openHaddit(int hadditNum)=0;

    virtual void scrollDown()=0;
    virtual void scrollUp()=0;

    virtual void reloadCurrentPage()=0;

protected:
    LibraryBook::Ptr m_book;
};

#endif // BOOKVIEWBASE_H
