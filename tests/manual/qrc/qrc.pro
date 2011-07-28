TEMPLATE = app
TARGET = tst_qrc
CONFIG += qt warn_on

INSTALL_DIRS = qml
CONFIG += qt3d_deploy_qml qt3dquick_deploy_pkg
include(../../../pkg.pri)
qtcAddDeployment()

SOURCES += main.cpp

OTHER_FILES += \
    qml/tst_cube.qml

RESOURCES += \
    qrc.qrc

