#!/bin/bash
#
# Script to build and package evilpixie on windows with MSYS2 MinGW x64.

set -e

# First, compile it.

BUILD=$(mktemp -d)
meson setup --buildtype release ${BUILD}
meson compile -C ${BUILD}

# Now package it into a dir.

VER=$(grep VERSION_STRING src/version.h | grep -o "[0-9.]*")

PACKAGE=$(mktemp -d)/evilpixie-${VER}
mkdir -p ${PACKAGE}

cp -r data ${PACKAGE}/
cp ${BUILD}/evilpixie.exe ${PACKAGE}

echo "run windeployqt to copy in DLLs and Qt gubbins..."
windeployqt.exe --compiler-runtime --no-translations ${PACKAGE}/evilpixie.exe

echo "copy in extra DLLs missed by windeployqt..."
for l in `ldd ${PACKAGE}/evilpixie.exe | grep -vi WINDOWS | awk '{print $3}'`; do cp -n $l ${PACKAGE}; done

echo "Build successful: ${PACKAGE}"

