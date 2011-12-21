TEMPLATE = app
TARGET = forest_qml
QT += declarative quick
CONFIG += qt warn_on

SOURCES += main.cpp

INSTALL_DIRS = qml
mt: INSTALL_FILES = mt.qml

OTHER_FILES += \
    forest.rc

RC_FILE = forest.rc

RESOURCES += \
    forest.qrc
