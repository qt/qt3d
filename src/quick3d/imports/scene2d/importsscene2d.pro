CXX_MODULE = qml
TARGET = qtquickscene2dplugin
TARGETPATH = QtQuick/Scene2D
IMPORT_VERSION = 2.$$QT_MINOR_VERSION
DEFINES += BUILD_QT3D_MODULE

QT += qml quick 3dcore 3drender 3drender-private 3dinput 3dlogic 3dquickscene2d 3dquickscene2d-private

HEADERS += \
    qtquickscene2dplugin.h \

SOURCES += \
    qtquickscene2dplugin.cpp \

OTHER_FILES += qmldir

load(qml_plugin)
