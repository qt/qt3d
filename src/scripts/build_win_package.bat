
:: Before running this script ensure that the %PATH% has the location of:
::    - the correct qmake for the Qt to build against
::    - the compiler and build tools, eg nmake
::         for these first two, use the C:\Qt\<ver>\bin\qtvars.bat vsvars command
::    - NSIS compiler, "makensis"
::         set PATH=%PATH%;"C:\Program Files (x86)\NSIS"
:: Run this script from the root of a complete source tree of Qt3D, eg
::    mkdir C:\build\qt
::    cd C:\build\qt
::    mkdir quick3d_mk_win_pkg
::    echo .git >exclude
::    xcopy /EXCLUDE:exclude /S C:\depot\qt\quick3d quick3d_mk_win_pkg
::    cd quick3d_mk_win_pkg

:: Use jom if possible - put jom in the path if you want faster compiles
where jom.exe
if %ERRORLEVEL% NEQ 0 (
    SET MAKE_PRG=nmake
) else (
    SET MAKE_PRG=jom
)

qmake.exe quick3d.pro -spec win32-msvc2008 CONFIG+=release CONFIG+=package
:: This has to be the full path, but without the drive letter...
set INSTALL_ROOT=%CD:~2%\tmp
%MAKE_PRG% install
qmake -query QT_VERSION >tmp\qt_version
set /P QT_VERSION= <tmp\qt_version
makensis /DQT_VERSION=%QT_VERSION% /NOCD src\scripts\build_win_package.nsi
