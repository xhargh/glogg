#!/bin/bash

# Build glogg for OSX and make a DMG installer
# (uses https://github.com/LinusU/node-appdmg)
#
# brew install node
# npm install -g appdmg
#
# QTDIR is built -static

QTDIR=$HOME/Qt/5.13.2/clang_64

make clean
#if [ ! -d "$BOOSTDIR" ]; then
#    echo $BOOSTDIR not found.
#    exit 1
#elif [ -z "$VERSION" ]; then
if [ -z "$VERSION" ]; then
    echo Please specify a version to build: VERSION=1.2.3 $0
    exit 1
else
    # $QTDIR/bin/qmake glogg-io.pro LIBS+="-dead_strip" CONFIG+="release no-dbus version_checker" VERSION="$VERSION"
    $QTDIR/bin/qmake glogg-io.pro LIBS+="-dead_strip" CONFIG+="release no-dbus" VERSION="$VERSION"

fi
make -j8
$QTDIR/bin/macdeployqt release/glogg-io.app -dmg
# dsymutil release/glogg-io.app/Contents/MacOS/glogg-io
mv release/glogg-io.app/Contents/MacOS/glogg-io.dSYM release/glogg-io-$VERSION.dSYM

rm glogg-io_${VERSION}_installer.dmg
sed -e "s/\"glogg-io\"/\"glogg-io $VERSION\"/" osx_installer.json >osx_${VERSION}_installer.json
appdmg osx_${VERSION}_installer.json glogg-io_${VERSION}_installer.dmg
rm osx_${VERSION}_installer.json
