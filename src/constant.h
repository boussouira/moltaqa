#ifndef CONSTANT_H
#define CONSTANT_H

#include <QObject>
/*
 Important: if you are using QtCreator you must set the editor's encoding to UTF-8
 you can edit it in Projects -> Editor Settings -> Defaut file Encoding -> UTF-8
*/

#define DATABASEPATH "Quran.db"

#define BASMALA         QObject::trUtf8("بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ")
#define SORAT           QObject::trUtf8("سورة")
#define RAKEM           QObject::trUtf8("رقم")
#define AYANUMBER       QObject::trUtf8("الاية")
#define ANASS           QObject::trUtf8("النص")
#define SEARCHRESULTS   QObject::trUtf8("نتائج البحث %1 :")

// Defaut font settings
#ifdef WIN32
    #define DEFAUTFONT "Traditional Arabic,20,-1,5,50,0,0,0,0,0"
    #define CONFIGFILE "settings.ini"
#else
    #define DEFAUTFONT "KacstBook,26,-1,5,50,0,0,0,0,0"
    #define CONFIGFILE "settings.cfg"
#endif

#endif // CONSTANT_H
