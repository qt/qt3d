TEMPLATE = app
TARGET = forest_qml
QT += declarative
CONFIG += qt warn_on

SOURCES += main.cpp

include(../../../pkg.pri)

OTHER_FILES += \
    forest.rc

RC_FILE = forest.rc

RESOURCES += \
    forest.qrc
