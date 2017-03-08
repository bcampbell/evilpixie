#!/bin/bash

set -e

BUILDDIR=evilpixie


mkdir -p $BUILDDIR
cp -r ../data $BUILDDIR

# /mingw64/bin





echo "Build exe..."
    
pushd .. >/dev/null
qmake evilpixie.pro
make
popd >/dev/null

cp ../release/evilpixie.exe $BUILDDIR

echo "run windeployqt to copy in DLLs and Qt gubbins..."
windeployqt.exe --compiler-runtime --no-translations $BUILDDIR/evilpixie.exe


echo "copy in extra DLLs missed by windeployqt..."
DLLS="zlib1.dll libpng16-16.dll libgif-7.dll libintl-8.dll libicuin57.dll libicuuc57.dll libicudt57.dll libharfbuzz-0.dll libpcre16-0.dll libpcre-1.dll libglib-2.0-0.dll libgraphite2.dll libfreetype-6.dll libbz2-1.dll libiconv-2.dll"

for dll in $DLLS; do
    full=`which $dll`
    cp $full $BUILDDIR/
    echo $full
done

