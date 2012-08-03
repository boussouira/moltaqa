#!/bin/bash

if [ -z $1 ]
then
    echo "    Usage: $0 <version string>"
    exit 0
fi

if [ ! -d "src" ] || [ ! -e "moltaqa-lib.pro" ]
then
    echo "You should run this script in the source tree direcory."
    exit 0
fi

version="$1"

currentRev=`grep '#define APP_UPDATE_REVISION' src/utils.h | cut -d' ' -f3`
let 'newRev=currentRev+1'

echo [*] Version: $version
echo [*] Revision: $newRev

echo

echo [+] Change src/utils.h
sed -i "s/\(APP_VERSION_STR\)\W\+\"[^\"]\+\"/\1 \"$version\"/" src/utils.h

sed -i 's/\(#define APP_UPDATE_REVISION\)\W[0-9]\+/\1 '$newRev'/' src/utils.h

echo [+] Change build-deb.sh
sed -i 's/\(APP_VERSION=\)"[^"]\+"/\1"'$version'"/' build-deb.sh

echo [+] Change build-win.bat
sed -i 's/\(set OUTDIR=moltaqa-lib-\)[^\W]\+/\1'$version'/' build-win.bat

echo [+] Change installer.iss
sed -i 's/\(AppVerName=[^ ]\+ [^ ]\+\) [^\w]\+/\1 '$version'/' installer.iss

echo

echo [*] "done"
