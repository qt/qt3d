TEMPLATE = app
TARGET = tst_qrc

# This manual test is to confirm the functionality of running
# Qt3D QML apps from a Qt resource (or *.qrc) file.
#
# There is no point in installing or running this test on
# platforms which are only ever intended to run QML apps,
# and also since the point of qrc is that QML is packaged
# into a resource file (and thus does not require deployment)
# hence the pkg.pri logic is not included here.

!package: DESTDIR = ../../../bin

QT += 3d quick

SOURCES += main.cpp

INSTALL_DIRS = qml

OTHER_FILES += \
    qml/tst_qrc.qml

RESOURCES += \
    qrc.qrc

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
