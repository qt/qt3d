:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
:: Contact: http://www.qt-project.org/legal
::
:: This file is part of the Qt3D module of the Qt Toolkit.
::
:: $QT_BEGIN_LICENSE:LGPL$
:: Commercial License Usage
:: Licensees holding valid commercial Qt licenses may use this file in
:: accordance with the commercial license agreement provided with the
:: Software or, alternatively, in accordance with the terms contained in
:: a written agreement between you and Digia.  For licensing terms and
:: conditions see http://qt.digia.com/licensing.  For further information
:: use the contact form at http://qt.digia.com/contact-us.
::
:: GNU Lesser General Public License Usage
:: Alternatively, this file may be used under the terms of the GNU Lesser
:: General Public License version 2.1 as published by the Free Software
:: Foundation and appearing in the file LICENSE.LGPL included in the
:: packaging of this file.  Please review the following information to
:: ensure the GNU Lesser General Public License version 2.1 requirements
:: will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
::
:: In addition, as a special exception, Digia gives you certain additional
:: rights.  These rights are described in the Digia Qt LGPL Exception
:: version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
::
:: GNU General Public License Usage
:: Alternatively, this file may be used under the terms of the GNU
:: General Public License version 3.0 as published by the Free Software
:: Foundation and appearing in the file LICENSE.GPL included in the
:: packaging of this file.  Please review the following information to
:: ensure the GNU General Public License version 3.0 requirements will be
:: met: http://www.gnu.org/copyleft/gpl.html.
::
::
:: $QT_END_LICENSE$
::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Before running this script ensure that the %PATH% has the location of:
::    - the correct qmake for the Qt to build against
::    - the compiler and build tools, eg nmake
::         for these first two, use the C:\Qt\<ver>\bin\qtvars.bat vsvars command
::    - NSIS compiler, "makensis"
::         set PATH=%PATH%;"C:\Program Files (x86)\NSIS"
::         NSIS needs the custom license plugin - if its not installed get it
::         from http://nsis.sourceforge.net/CustomLicense_plug-in
:: Run this script from the root of a complete source tree of Qt3D, eg
::    mkdir C:\build\qt
::    cd C:\build\qt
::    mkdir quick3d_mk_win_pkg
::    echo .git >exclude
::    xcopy /EXCLUDE:exclude /S C:\depot\qt\quick3d quick3d_mk_win_pkg
::    cd quick3d_mk_win_pkg
:: Use jom if possible - put jom in the path if you want faster compiles
::    set PATH=%PATH%;C:\QtSDK\QtCreator\bin
::    src\scripts\build_win_package.bat

where jom.exe
if %ERRORLEVEL% NEQ 0 (
    SET MAKE_PRG=nmake
) else (
    SET MAKE_PRG=jom
)

mkdir tmp

qmake -query QT_VERSION >tmp\qt_version
set /P QT_VERSION= <tmp\qt_version

qmake -query QT_INSTALL_PREFIX >tmp\qt_prefix
set /P QT_PREFIX= <tmp\qt_prefix
set QT_PREFIX_PATH=%QT_PREFIX:~2%

:: On windows if the qt3d.prf and qt3dquick.prf exist in the Qt, the build will fail
:: with impenetrable link errors.  This should not happen but might if you are trying
:: to debug the installer scripts, and do something odd.
del %QTDIR%\mkspecs\features\qt3d*

qmake.exe quick3d.pro -spec win32-msvc2008 CONFIG+=release CONFIG+=package

:: This has to be the full path, but without the drive letter...
set INSTALL_ROOT=%CD:~2%\tmp
%MAKE_PRG% install
%MAKE_PRG% docs
makensis /DQT_VERSION=%QT_VERSION% /DQT_PREFIX_PATH=%QT_PREFIX_PATH% /NOCD src\scripts\build_win_package.nsi
