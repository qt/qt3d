TEMPLATE = app
TARGET = forest_qml
CONFIG += qt warn_on
!package: CONFIG += qt3dquick

SOURCES += main.cpp

CONFIG += qt3dquick_deploy_pkg
include(../../../pkg.pri)

OTHER_FILES += \
    forest.rc

RC_FILE = forest.rc

RESOURCES += \
    forest.qrc
