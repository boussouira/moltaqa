#ifndef CLCONSTANTS_H
#define CLCONSTANTS_H


#define MAX_SEGMENT 2

#define IS_UPPER(c) (0x41 <= c &&  c <= 0x5A)
#define IS_LOWER(c) (0x61 <= c &&  c <= 0x7A)
#define IS_LATIN(c) (0xC0 <= c && c <= 0xFF)
#define IS_ASCII(c) (IS_LOWER(c) || IS_UPPER(c) || IS_LATIN(c))
#define IS_NUMBER(c) (0x30 <= c &&  c <= 0x39)
#define IS_ARABIC_CHAR(c) (0x0621 <= c &&  c <= 0x06ED)

#define PAGE_ID_FIELD _T("page")
#define BOOK_ID_FIELD _T("book")
#define PAGE_TEXT_FIELD _T("text")
#define TITLE_TEXT_FIELD _T("title")
#define TITLE_ID_FIELD _T("tid")
#define FOOT_NOTE_FIELD _T("fn")
#define QURAN_SORA_FIELD _T("sora")
#define AUTHOR_DEATH_FIELD _T("death")


#endif // CLCONSTANTS_H
