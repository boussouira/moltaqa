#!/bin/bash

APP_NAME="moltaqa-lib"
APP_VERSION="0.8"
APP_DIR_NAME="${APP_NAME}-${APP_VERSION}"
APP_FILES="moltaqa-lib.pro  Doxyfile share  src  test"
TEMP_DIR="temp-build-deb"
SOURCE_ARCHIVE="${APP_NAME}-${APP_VERSION}.tar.gz"
SOURCE_ARCHIVE_ORIG="${APP_NAME}_${APP_VERSION}.orig.tar.gz"

echo "[*] Remove temp dir: $TEMP_DIR"
rm -rvf $TEMP_DIR >> /dev/null

echo "[*] Create temp dir: $TEMP_DIR"
mkdir $TEMP_DIR

echo "[*] Create source archive: $SOURCE_ARCHIVE"
tar -zcvf $TEMP_DIR/$SOURCE_ARCHIVE $APP_FILES >> /dev/null

echo "[*] Create orign archive: $SOURCE_ARCHIVE_ORIG"
cp -v $TEMP_DIR/$SOURCE_ARCHIVE $TEMP_DIR/$SOURCE_ARCHIVE_ORIG  >> /dev/null

echo "[*] Create application directory: $APP_DIR_NAME"
mkdir $TEMP_DIR/$APP_DIR_NAME

echo "[*] Copy files to application dir"
cp -fvr $APP_FILES debian $TEMP_DIR/$APP_DIR_NAME >> /dev/null

echo "[*] Enter application directory"
cd $TEMP_DIR/$APP_DIR_NAME

echo "[*] Start building deb"
debuild >> /dev/null

cd ..
echo "[*] Generated deb:" *.deb

echo "[*] Done"
