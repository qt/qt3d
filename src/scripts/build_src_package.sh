#!/bin/bash

usage() {
    cat <<USAGE
usage:
  $0 <refspec> <version>

  Creates tar and zip source package from <refspec> and documentation-zip from current checkout.
  Run the command from inside the depot root.

  Generated package files and directories are named after <version>.

  Make sure the bin path for a valid Qt with qdoc3 is in the $PATH.

  example:
    env PATH=/path/to/Qt/bin:$PATH src/scripts/createSrcAndDocPackage.sh origin/2.0.0 2.0.0-rc1
USAGE
    exit 1
}

## Command line parameters
if [[ $# != 2 ]]; then
    usage;
fi

## Is the qmake binary in the path?
/usr/bin/type -P qmake 2>&1 >/dev/null || usage

echo "Using Qt $(qmake -query QT_INSTALL_BINS)/qdoc3 to build doc"
test -f quick3d.pro || usage

BRANCH=$1
VERSION=$2
echo "Creating tar archive..."
git archive --format=tar --prefix=qtquick3d-${VERSION}-src/ ${BRANCH} | gzip > ../qtquick3d-${VERSION}-src.tar.gz || exit 1
echo "Creating zip archive..."
git archive --format=zip --prefix=qtquick3d-${VERSION}-src/ ${BRANCH} > ../qtquick3d-${VERSION}-src.zip || exit 1
echo "Creating documentation..."
rm -r doc/html
qmake quick3d.pro -spec macx-g++ CONFIG+=package >/dev/null || exit 1
make docs || exit 1
cd doc
cp -r html qtquick3d-${VERSION}
zip -r ../qtquick3d-${VERSION}-doc.zip qtquick3d-${VERSION} >/dev/null
rm -r qtquick3d-${VERSION}
