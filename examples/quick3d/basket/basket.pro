TEMPLATE = app
TARGET = basket_qml
QT += declarative quick
CONFIG += qt warn_on

SOURCES += main.cpp

INSTALL_DIRS = qml
mt: INSTALL_FILES = mt.qml

CONFIG += qt3d_deploy_qml
include(../../../pkg.pri)
qtcAddDeployment()

OTHER_FILES += \
    basket_qml.rc

RC_FILE = basket_qml.rc

RESOURCES += \
    basket.qrc

