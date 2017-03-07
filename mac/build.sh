#!/bin/bash
set -e

# build executable (and bundle structure)

pushd .. >/dev/null
qmake CONFIG+=release evilpixie.pro
make clean
make
popd >/dev/null


BUNDLE=../evilpixie.app

# add qt gubbins to bundle

echo "running macdeployqt..."

macdeployqt $BUNDLE

echo "add data files to bundle..."
cp -r ../data/* $BUNDLE/Contents/Resources/

echo "add info.plist to bundle..."
cp Info.plist $BUNDLE/Contents/

# now build icon and add to bundle
# requires imagemagick

echo "building icon..."

ICONSET=/tmp/evilpixie.iconset
ICNS=/tmp/evilpixie.icns
IN=../icon_128x128.png

mkdir $ICONSET
cp ../icon_128x128.png $ICONSET/
convert $IN -resize 16x16\!  $ICONSET/icon_16x16.png
convert $IN -resize 32x32\!  $ICONSET/icon_16x16@2x.png
convert $IN -resize 32x32\!  $ICONSET/icon_32x32.png
convert $IN -resize 64x64\!  $ICONSET/icon_32x32@2x.png
#sips -z 128 128   $IN --out $ICONSET/icon_128x128.png
#sips -z 256 256   $IN --out $ICONSET/icon_128x128@2x.png
#sips -z 256 256   $IN --out $ICONSET/icon_256x256.png
#sips -z 512 512   $IN --out $ICONSET/icon_256x256@2x.png
#sips -z 512 512   $IN --out $ICONSET/icon_512x512.png
#cp $IN $ICONSET/icon_512x512@2x.png
iconutil --convert icns --output $BUNDLE/Contents/Resources/evilpixie.icns $ICONSET
rm -r $ICONSET


echo "done. Now sign and pack into dmg:"
echo "codesign --deep -s \"Developer ID Application\" $BUNDLE"
echo "hdiutil create -srcfolder $BUNDLE evilpixie.dmg"

# note codesign doesn't work by default over ssh (needs some keychain whitelist magic)

