#include "bookviewbase.h"

BookViewBase::BookViewBase(LibraryBook::Ptr book, QWidget *parent) :
    QWidget(parent)
{
    m_book = book;
}

LibraryBook::Ptr BookViewBase::book()
{
    return m_book;
}

void BookViewBase::toggleIndexWidget()
{
}

void BookViewBase::saveSettings()
{
}

void BookViewBase::loadSettings()
{
}

void BookViewBase::openPageID(int id)
{
    openPage(id);
}
