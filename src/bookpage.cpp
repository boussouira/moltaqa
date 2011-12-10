#include "bookpage.h"

BookPage::BookPage()
{
    sora = 0;
    aya = 0;
    ayatCount  = 0;
    page = 0;
    pageID = 0;
    part = 0;
    haddit = 0;
    titleID = 0;
}

BookPage *BookPage::clone()
{
    BookPage *p = new BookPage();

    p->sora = sora;
    p->aya = aya;
    p->ayatCount = ayatCount;
    p->page = page;
    p->pageID = pageID;
    p->part = part;
    p->haddit = haddit;
    p->titleID = titleID;

    p->text = text;
    p->title = title;

    return p;
}
