#!/bin/bash
#############################################################################
##
## Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
## Contact: http://www.qt-project.org/legal
##
## This file is part of the Qt3D module of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:LGPL$
## Commercial License Usage
## Licensees holding valid commercial Qt licenses may use this file in
## accordance with the commercial license agreement provided with the
## Software or, alternatively, in accordance with the terms contained in
## a written agreement between you and Digia.  For licensing terms and
## conditions see http://qt.digia.com/licensing.  For further information
## use the contact form at http://qt.digia.com/contact-us.
##
## GNU Lesser General Public License Usage
## Alternatively, this file may be used under the terms of the GNU Lesser
## General Public License version 2.1 as published by the Free Software
## Foundation and appearing in the file LICENSE.LGPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU Lesser General Public License version 2.1 requirements
## will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## In addition, as a special exception, Digia gives you certain additional
## rights.  These rights are described in the Digia Qt LGPL Exception
## version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
##
## GNU General Public License Usage
## Alternatively, this file may be used under the terms of the GNU
## General Public License version 3.0 as published by the Free Software
## Foundation and appearing in the file LICENSE.GPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU General Public License version 3.0 requirements will be
## met: http://www.gnu.org/copyleft/gpl.html.
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
test -f quick3d.pro || usage

BRANCH=$1
VERSION=$2
echo "Creating tar archive..."
git archive --format=tar --prefix=qtquick3d-${VERSION}-src/ ${BRANCH} | gzip > ../qtquick3d-${VERSION}-src.tar.gz || exit 1
echo "Creating zip archive..."
git archive --format=zip --prefix=qtquick3d-${VERSION}-src/ ${BRANCH} > ../qtquick3d-${VERSION}-src.zip || exit 1
echo "Creating documentation..."
rm -r -f doc/html
qmake quick3d.pro -spec macx-g++ CONFIG+=package >/dev/null || exit 1
make docs || exit 1
cd doc
cp -r html qtquick3d-${VERSION}
zip -r ../../qtquick3d-${VERSION}-doc.zip qtquick3d-${VERSION} >/dev/null
rm -r qtquick3d-${VERSION}
