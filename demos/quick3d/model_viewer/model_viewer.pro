TEMPLATE = app
TARGET = model_viewer
CONFIG += qt warn_on
!package: CONFIG += qt3dquick

SOURCES += main.cpp

include(../../../qt3dquick_pkg_dep.pri)
include(../../../qml_pkg.pri)

OTHER_FILES += \
    model_viewer.rc

RC_FILE = model_viewer.rc
