TEMPLATE = app
TARGET = robo_bounce
QT += declarative quick

SOURCES += main.cpp

INSTALL_DIRS = qml
mt: INSTALL_FILES = mt.qml

CONFIG += qt3d_deploy_qml
include(../../../pkg.pri)
qtcAddDeployment()

OTHER_FILES += \
    robo_bounce.rc

RC_FILE = robo_bounce.rc

RESOURCES += \
    robo_bounce.qrc

