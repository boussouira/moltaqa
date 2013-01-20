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

void BookViewBase::hideIndexWidget()
{
}

void BookViewBase::saveSettings()
{
}

void BookViewBase::loadSettings()
{
}
