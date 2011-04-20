#!/bin/bash

set -e

APP=QtQuick3D

PRO=quick3d.pro

if [[ -z "$1" ]]; then
    echo "Usage: build_bundle.sh 1.2.3"
    echo "(where 1.2.3 is the version number)"
    exit 1;
fi

if [[ ! -f ${SRC}/quick3d.pro ]]; then
    echo "Could not find quick3d.pro in current directory."
    echo "This script should be run from the root of the Qt3D source tree."
    exit 1;
fi

VER=$1

RELEASE_DIR="../${APP}-${VER}"

rm -R -f ${RELEASE_DIR}

mkdir -p ${RELEASE_DIR}

echo "Attempting to build ${APP}..."
qmake -makefile -spec macx-g++ -r CONFIG+=release CONFIG+=package quick3d.pro
make
INSTALL_ROOT=tmp make install

exit 0

DMG="../${APP}-${VER}.dmg"

# echo "New distributable ready in ${DMG}"

echo "Trying to use hdiutil to make a self deploying disk image"
hdiutil create -srcfolder ${RELEASE_DIR} ${DMG}
hdiutil internet-enable -yes ${DMG}
