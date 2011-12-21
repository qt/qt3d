TEMPLATE = app
TARGET = tst_qrc
CONFIG += qt warn_on

QT += qt3dquick

SOURCES += main.cpp

INSTALL_DIRS = qml

OTHER_FILES += \
    qml/tst_cube.qml

RESOURCES += \
    qrc.qrc

