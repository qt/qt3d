#!/bin/bash
#############################################################################
##
## Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
## All rights reserved.
## Contact: Nokia Corporation (qt-info@nokia.com)
##
## This file is part of the QtQuick3D module of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:LGPL$
## GNU Lesser General Public License Usage
## This file may be used under the terms of the GNU Lesser General Public
## License version 2.1 as published by the Free Software Foundation and
## appearing in the file LICENSE.LGPL included in the packaging of this
## file. Please review the following information to ensure the GNU Lesser
## General Public License version 2.1 requirements will be met:
## http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## In addition, as a special exception, Nokia gives you certain additional
## rights. These rights are described in the Nokia Qt LGPL Exception
## version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
##
## GNU General Public License Usage
## Alternatively, this file may be used under the terms of the GNU General
## Public License version 3.0 as published by the Free Software Foundation
## and appearing in the file LICENSE.GPL included in the packaging of this
## file. Please review the following information to ensure the GNU General
## Public License version 3.0 requirements will be met:
## http://www.gnu.org/copyleft/gpl.html.
##
## Other Usage
## Alternatively, this file may be used in accordance with the terms and
## conditions contained in a signed written agreement between you and Nokia.
##
##
##
##
##
## $QT_END_LICENSE$
##
#############################################################################


usage() {
    cat <<USAGE
usage:
  $0 <refspec> <version>

  Creates tar and zip source package from <refspec> and documentation-zip from current checkout.
  Run it from inside [a copy of] the git checkout of qt3d source.

  Generated package files and directories are named after <version>.

  Make sure the bin path for a valid Qt with qdoc3 is in the $PATH.

  example:
    env PATH=/path/to/Qt/bin:$PATH src/scripts/build_src_package.sh origin/tp1 1.0-tp1
USAGE
    exit 1
}

## Command line parameters
if [[ $# != 2 ]]; then
    usage;
fi

set -e

QDOC3=$(qmake -query QT_INSTALL_BINS)/qdoc3

echo "Using Qt ${QDOC3} to build doc"
test -f qt3d.pro || usage

BRANCH=$1
VERSION=$2
echo "Creating tar archive..."
git archive --format=tar --prefix=qtquick3d-${VERSION}-src/ ${BRANCH} | gzip > ../qtquick3d-${VERSION}-src.tar.gz || exit 1
echo "Creating zip archive..."
git archive --format=zip --prefix=qtquick3d-${VERSION}-src/ ${BRANCH} > ../qtquick3d-${VERSION}-src.zip || exit 1
echo "Creating documentation..."
rm -r -f doc/html
qmake qt3d.pro -spec macx-g++ CONFIG+=package >/dev/null || exit 1
make docs || exit 1
cd doc
cp -r html qtquick3d-${VERSION}
zip -r ../../qtquick3d-${VERSION}-doc.zip qtquick3d-${VERSION} >/dev/null
rm -r qtquick3d-${VERSION}
