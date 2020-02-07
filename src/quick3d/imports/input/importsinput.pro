CXX_MODULE = qml
TARGET = quick3dinputplugin
TARGETPATH = Qt3D/Input
IMPORT_VERSION = 2.$$QT_MINOR_VERSION
DEFINES += BUILD_QT3D_MODULE

QT += core-private qml 3dcore 3dinput 3dinput-private 3dquickinput-private

OTHER_FILES += qmldir

HEADERS += \
    qt3dquick3dinputplugin.h

SOURCES += \
    qt3dquick3dinputplugin.cpp

qtHaveModule(gamepad): DEFINES += HAVE_QGAMEPAD

load(qml_plugin)
