#ifndef CONSTANT_H
#define CONSTANT_H

#include <QObject>
/*
 Important: if you are using QtCreator you must set the editor's encoding to UTF-8
 you can edit it in Projects -> Editor Settings -> Defaut file Encoding -> UTF-8
*/

#define DATABASEPATH "quran.db"

#define QURAN           QObject::trUtf8("القرآن الكريم")
#define BASMALA         QObject::trUtf8("بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ")
#define SORAT           QObject::trUtf8("سورة")
#define RAKEM           QObject::trUtf8("رقم")
#define AYANUMBER       QObject::trUtf8("الاية")
#define ANASS           QObject::trUtf8("النص")
#define SEARCHRESULTS   QObject::trUtf8("نتائج البحث %1 :")
#define OPENSORA        QObject::trUtf8("فتح السورة")
#define OPENSORAINTAB   QObject::trUtf8("فتح في تبويب جديد")

#define CONFIGFILE "settings.ini"

#endif // CONSTANT_H
