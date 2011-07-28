TEMPLATE = app
TARGET = basket_qml
CONFIG += qt warn_on

CONFIG += qt3dquick_deploy_pkg
include(../../../pkg.pri)

SOURCES += main.cpp

OTHER_FILES += \
    basket_qml.rc

RC_FILE = basket_qml.rc

RESOURCES += \
    basket.qrc

