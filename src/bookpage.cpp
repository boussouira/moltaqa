#include "bookpage.h"

BookPage::BookPage()
{
    clear();
}

void BookPage::clear()
{
    sora = 0;
    aya = 0;
    ayatCount = 0;
    page = 0;
    pageID = 0;
    part = 0;
    haddit = 0;
    titleID = 0;

    if(text.size())
        text.clear();

    if(title.size())
        title.clear();
}

BookPage *BookPage::clone()
{
    return new BookPage(*this);
}
