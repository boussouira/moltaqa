#!/bin/bash

APP_NAME="moltaqa-lib"
APP_VERSION="0.9a4"
APP_DIR_NAME="${APP_NAME}-${APP_VERSION}"
APP_FILES="moltaqa-lib.pro Doxyfile share bin src test"
TEMP_DIR="temp-build-deb"
RPM_TEMP_DIR="rpm-temp-dir"
OUTPUT_DIR="dist"
SOURCE_ARCHIVE="${APP_NAME}-${APP_VERSION}.tar.gz"
SOURCE_ARCHIVE_ORIG="${APP_NAME}_${APP_VERSION}.orig.tar.gz"
SOURCE_DIR=`pwd`
TEMP_DIR_PATH=$SOURCE_DIR/$TEMP_DIR

echo "********************************"
echo " Package: $APP_NAME"
echo " Version: $APP_VERSION"
echo "********************************"

echo "[*] Remove temp dir: $TEMP_DIR"
rm -rf $TEMP_DIR

echo "[*] Create temp directories"
mkdir $TEMP_DIR
mkdir -p $TEMP_DIR/$OUTPUT_DIR
mkdir -p $TEMP_DIR/$RPM_TEMP_DIR

echo "[*] Create source archive: $SOURCE_ARCHIVE"
tar -zcf $TEMP_DIR/$SOURCE_ARCHIVE $APP_FILES

echo "[*] Create orign archive: $SOURCE_ARCHIVE_ORIG"
cp $TEMP_DIR/$SOURCE_ARCHIVE $TEMP_DIR/$SOURCE_ARCHIVE_ORIG

echo "[*] Create application directory: $APP_DIR_NAME"
mkdir $TEMP_DIR/$APP_DIR_NAME

echo "[*] Copy files to application dir"
cp -fr $APP_FILES debian .git $TEMP_DIR/$APP_DIR_NAME

echo "[*] Enter application directory"
cd $TEMP_DIR/$APP_DIR_NAME

echo "[*] Start building deb"
debuild

cd $TEMP_DIR_PATH

echo "[*] Convert to rpm"
cd $RPM_TEMP_DIR

fakeroot alien --to-rpm ../*.deb

cd $TEMP_DIR_PATH

cp *.deb $OUTPUT_DIR
cp $RPM_TEMP_DIR/*.rpm $OUTPUT_DIR

echo "[*] Create archives"

cd $TEMP_DIR_PATH/$APP_DIR_NAME/debian/moltaqa-lib/
mv -v usr $APP_DIR_NAME

cp -v $TEMP_DIR_PATH/$APP_DIR_NAME/bin/moltaqa-lib.run $APP_DIR_NAME/bin

tar -zcf $TEMP_DIR_PATH/$OUTPUT_DIR/$APP_DIR_NAME.tar.gz $APP_DIR_NAME
#tar -jcf $TEMP_DIR_PATH/$OUTPUT_DIR/$APP_DIR_NAME.tar.bz2 *

echo "[*] Create md5 sums"

cd $TEMP_DIR_PATH/$OUTPUT_DIR
md5sum -b * > MD5SUMS

mkdir -p "$SOURCE_DIR/$OUTPUT_DIR/$APP_DIR_NAME"
cp -rv $TEMP_DIR_PATH/$OUTPUT_DIR/* "$SOURCE_DIR/$OUTPUT_DIR/$APP_DIR_NAME"

echo "[*] Generated files in $SOURCE_DIR/$OUTPUT_DIR/$APP_DIR_NAME"
cat $SOURCE_DIR/$OUTPUT_DIR/$APP_DIR_NAME/MD5SUMS

echo "[*] Remove build directory"
cd $SOURCE_DIR
rm -fr $TEMP_DIR

echo "[*] Done"

