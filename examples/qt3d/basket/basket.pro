TEMPLATE = app
TARGET = basket
CONFIG += qt warn_on
QT += 3d

include(../../../pkg.pri)

SOURCES = basketview.cpp main.cpp
HEADERS = basketview.h basket_data.h

OTHER_FILES += \
    basket.rc

RC_FILE = basket.rc
