#!/bin/bash

## Command line parameters
if [[ $# != 2 ]]; then
    cat <<USAGE
usage:
  $0 <refspec> <version>

  Creates tar and zip source package from <refspec> and documentation-zip from current checkout.
  Files and directories are named after <version>.
  example:
    $0 origin/2.0.0 2.0.0-rc1
USAGE
    exit 1
fi

BRANCH=$1
VERSION=$2
cd `dirname $0`/..
echo "Creating tar archive..."
git archive --format=tar --prefix=qtquick3d-${VERSION}-src/ ${BRANCH} | gzip > qtquick3d-${VERSION}-src.tar.gz || exit 1
echo "Creating zip archive..."
git archive --format=zip --prefix=qtquick3d-${VERSION}-src/ ${BRANCH} > qtquick3d-${VERSION}-src.zip || exit 1
echo "Creating documentation..."
rm -r doc/html
qmake -r && make docs || exit 1
cd doc
cp -r html qtquick3d-${VERSION}
zip -r ../qtquick3d-${VERSION}-doc.zip qtquick3d-${VERSION}
rm -r qtquick3d-${VERSION}
