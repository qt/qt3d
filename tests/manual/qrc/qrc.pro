TEMPLATE = app
TARGET = tst_qrc
CONFIG += qt warn_on

!package: CONFIG += qt3dquick

SOURCES += main.cpp

CONFIG += qt3d_deploy_qml qt3dquick_deploy_pkg
include(../../../pkg.pri)

OTHER_FILES += \
    qml/tst_cube.qml

RESOURCES += \
    qrc.qrc

