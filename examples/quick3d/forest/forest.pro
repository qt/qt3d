TEMPLATE = app
TARGET = forest_qml
CONFIG += qt warn_on

CONFIG += qt3dquick_deploy_pkg
include(../../../pkg.pri)

SOURCES += main.cpp

OTHER_FILES += \
    forest.rc

RC_FILE = forest.rc

RESOURCES += \
    forest.qrc
