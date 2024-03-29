#ifndef BOOKVIEWBASE_H
#define BOOKVIEWBASE_H

#include "librarybook.h"

#include <qwidget.h>

class AbstractBookReader;

class BookViewBase : public QWidget
{
    Q_OBJECT
public:
    BookViewBase(LibraryBook::Ptr book, QWidget *parent = 0);
    
    LibraryBook::Ptr book();

    virtual AbstractBookReader *bookReader()=0;

    virtual void toggleIndexWidget();

    virtual void saveSettings();
    virtual void loadSettings();

public slots:
    virtual void firstPage()=0;
    virtual void lastPage()=0;
    virtual void nextPage()=0;
    virtual void prevPage()=0;

    /**
     * @brief openPageID seem as openPage, so we can call this function from javascript envirement
    */
    virtual void openPageID(int id);
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
