TEMPLATE = app
TARGET = tst_translation
CONFIG += qt warn_on
!package: CONFIG += qt3dquick

!package: CONFIG += qt3dquick

SOURCES += main.cpp

include(../../../qt3dquick_pkg_dep.pri)
include(../../../qml_pkg.pri)
