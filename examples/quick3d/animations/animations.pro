TEMPLATE = app
TARGET = animations_qml
CONFIG += qt warn_on

CONFIG += qt3dquick_deploy_pkg
include(../../../pkg.pri)

SOURCES += main.cpp

OTHER_FILES += \
    animations_qml.rc \
    animations_qml.desktop \
    qml/animations.qml

RC_FILE = animations_qml.rc

RESOURCES += \
    animations.qrc

