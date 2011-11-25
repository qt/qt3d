TEMPLATE = app
TARGET = basket
CONFIG += qt warn_on

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

SOURCES = basketview.cpp main.cpp
HEADERS = basketview.h basket_data.h
RESOURCES = basket.qrc

OTHER_FILES += \
    basket.rc \
    basket.desktop

RC_FILE = basket.rc
