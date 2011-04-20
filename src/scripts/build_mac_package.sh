#!/bin/bash

set -e

APP=ethex2040

APP_BUNDLE=${APP}.app

SRC=$(dirname $0)

if [[ -z "${QTDIR}" ]]; then
    echo "Please ensure the \$QTDIR env var is set, eg"
    echo "export QTDIR=/Library/Frameworks"
fi

if [[ -z "$1" ]]; then
    echo "Usage: build_bundle.sh 1.2.3"
    echo "(where 1.2.3 is the version number)"
    exit 1;
fi

if [[ ! -f ${SRC}/${APP}.pro ]]; then
    echo "Could not find ${APP}.pro in ${SRC}."
    echo "This script should be in the ${APP} source tree."
    echo "Run this script from the root of the ${APP} build tree, eg:"
    echo "cd ~/build/${APP}"
    echo "../../src/${APP}/build_bundle.sh 1.2.3"
    exit 1;
fi

VER=$1

RELEASE_DIR="../${APP}-${VER}"

rm -R -f ${RELEASE_DIR}

mkdir -p ${RELEASE_DIR}

rm -R -f ${APP_BUNDLE}

echo "Attempting to build ${APP_BUNDLE}..."
qmake -makefile -spec macx-g++ -config release ${SRC}/${APP}.pro
make clean
make

# First we create a Frameworks directory inside the bundle. This follows
# the Mac OS X application convention. We then copy the frameworks into
# the new directory. Since frameworks contain symbolic links, and we want
# to preserve them, we use the -R option.

mkdir -p ${APP_BUNDLE}/Contents/Frameworks/QtCore.framework/Versions/4
mkdir -p ${APP_BUNDLE}/Contents/Frameworks/QtCore.framework/Contents
cp -v ${QTDIR}/QtCore.framework/Versions/4/QtCore \
     ${APP_BUNDLE}/Contents/Frameworks/QtCore.framework/Versions/4
cp -v ${QTDIR}/QtCore.framework/Contents/Info.plist \
     ${APP_BUNDLE}/Contents/Frameworks/QtCore.framework/Contents
(cd ${APP_BUNDLE}/Contents/Frameworks/QtCore.framework/Versions \
   && ln -s 4 4.0 && ln -s 4 Current)
(cd ${APP_BUNDLE}/Contents/Frameworks/QtCore.framework \
   && ln -s Versions/4/QtCore)

mkdir -p ${APP_BUNDLE}/Contents/Frameworks/QtGui.framework/Versions/4
mkdir -p ${APP_BUNDLE}/Contents/Frameworks/QtGui.framework/Contents
cp -v ${QTDIR}/QtGui.framework/Versions/4/QtGui \
     ${APP_BUNDLE}/Contents/Frameworks/QtGui.framework/Versions/4
cp -v ${QTDIR}/QtGui.framework/Contents/Info.plist \
     ${APP_BUNDLE}/Contents/Frameworks/QtGui.framework/Contents
(cd ${APP_BUNDLE}/Contents/Frameworks/QtGui.framework/Versions \
   && ln -s 4 4.0 && ln -s 4 Current)
(cd ${APP_BUNDLE}/Contents/Frameworks/QtGui.framework \
   && ln -s Versions/4/QtGui)

# Then we run install_name_tool to set the identification names for the
# frameworks. The first argument after -id is the new name, and the second
# argument is the framework which identification we wish to change. The
# text @executable_path is a special dyld variable telling dyld to start
# looking where the executable is located. The new names specifies that
# these frameworks will be located "one directory up and over" in the
# Frameworks directory.

install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
        ${APP_BUNDLE}/Contents/Frameworks/QtCore.framework/Versions/4/QtCore
install_name_tool -id @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
        ${APP_BUNDLE}/Contents/Frameworks/QtGui.framework/Versions/4/QtGui

# Now, the dynamic linker knows where to look for QtCore and QtGui. Then we
# must make the application aware of the library locations as well using
# install_name_tool's -change mode. This basically comes down to string replacement,
# to match the identification names that we set for the frameworks.

install_name_tool -change QtCore.framework/Versions/4/QtCore \
         @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
         ${APP_BUNDLE}/Contents/MacOS/${APP}
install_name_tool -change QtGui.framework/Versions/4/QtGui \
         @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
         ${APP_BUNDLE}/Contents/MacOS/${APP}

# Finally, since the QtGui framework depends on QtCore, we must remember to
# change the references for it:

install_name_tool -change QtCore.framework/Versions/4/QtCore \
         @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
         ${APP_BUNDLE}/Contents/Frameworks/QtGui.framework/Versions/4/QtGui

# This command should list the new dependencies for the application, which should
# all be localized to the bundle.
echo "Check that the dependencies as shown by otool are all in the bundle"
otool -L ${APP_BUNDLE}/Contents/MacOS/${APP}

rm -R -f ${RELEASE_DIR}/${APP_BUNDLE}
mv ${APP_BUNDLE} ${RELEASE_DIR}
cp ${SRC}/README.html ${RELEASE_DIR}
(cd ${RELEASE_DIR} && ln -s -f /Applications)

echo "Bundle completed in ${RELEASE_DIR}"

make distclean

DMG="../${APP}-${VER}.dmg"

# echo "New distributable ready in ${DMG}"

echo "Trying to use hdiutil to make a self deploying disk image"
hdiutil create -srcfolder ${RELEASE_DIR} ${DMG}
hdiutil internet-enable -yes ${DMG}
