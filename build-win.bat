@echo off

set QTDIR=C:/Qt/qt4-build
set CLUCENEDIR=C:/clucene-vc
set OUTDIR=moltaqa-lib-1.0b1

echo Create directory %OUTDIR%
mkdir %OUTDIR%
cd %OUTDIR%

mkdir bin

echo Copy files:

echo -- Qt dlls
cp %QTDIR%/bin/phonon4.dll bin
cp %QTDIR%/bin/QtCore4.dll bin
cp %QTDIR%/bin/QtGui4.dll bin
cp %QTDIR%/bin/QtNetwork4.dll bin
cp %QTDIR%/bin/QtSql4.dll bin
cp %QTDIR%/bin/QtWebKit4.dll bin
cp %QTDIR%/bin/QtXmlPatterns4.dll bin
cp %QTDIR%/bin/QtXml4.dll bin

echo -- Qt Drivers

echo ---- SQL
mkdir bin\sqldrivers
cp %QTDIR%/plugins/sqldrivers/qsqlite4.dll bin/sqldrivers
cp %QTDIR%/plugins/sqldrivers/qsqlodbc4.dll bin/sqldrivers

echo ---- Images
mkdir bin\imageformats
cp %QTDIR%/plugins/imageformats/qgif4.dll bin/imageformats
cp %QTDIR%/plugins/imageformats/qico4.dll bin/imageformats
cp %QTDIR%/plugins/imageformats/qjpeg4.dll bin/imageformats
cp %QTDIR%/plugins/imageformats/qmng4.dll bin/imageformats
cp %QTDIR%/plugins/imageformats/qsvg4.dll bin/imageformats
cp %QTDIR%/plugins/imageformats/qtga4.dll bin/imageformats
cp %QTDIR%/plugins/imageformats/qtiff4.dll bin/imageformats

echo -- MSVC dlls
cp D:/Programming/albahhet-vc_release/msvcm90.dll bin
cp D:/Programming/albahhet-vc_release/msvcp90.dll bin
cp D:/Programming/albahhet-vc_release/msvcr90.dll bin
cp D:/Programming/albahhet-vc_release/Microsoft.VC90.CRT.manifest bin

echo -- CLucene dlls

cp %CLUCENEDIR%/bin/Release/clucene-contribs-lib.dll bin
cp %CLUCENEDIR%/bin/Release/clucene-core.dll bin
cp %CLUCENEDIR%/bin/Release/clucene-shared.dll bin

echo -- ZLib dll
cp D:/Programming/albahhet-vc_release/zlib1.dll bin

echo -- Application
cp D:/Programming/moltaqa-lib-vs-release/bin/moltaqa-lib.exe bin

mkdir share
mkdir share\moltaqa-lib

echo -- Share files
cp -r D:/Programming/moltaqa-lib/share/moltaqa-lib/data share/moltaqa-lib
cp -r D:/Programming/moltaqa-lib/share/moltaqa-lib/js share/moltaqa-lib
cp -r D:/Programming/moltaqa-lib/share/moltaqa-lib/locale share/moltaqa-lib
cp -r D:/Programming/moltaqa-lib/share/moltaqa-lib/styles share/moltaqa-lib
cp -r D:/Programming/moltaqa-lib/share/fonts share

cp D:/Programming/moltaqa-lib/share/moltaqa-lib/images/moltaqa-lib.ico moltaqa-lib.ico

cd ..

cp installer.iss %OUTDIR%

du -hs %OUTDIR%

echo Done
