TARGET   = Qt3DQuick

QT      += core-private gui-private qml qml-private 3dcore 3dcore-private

DEFINES += QT3DQUICK_LIBRARY

MODULE   = 3dquick

load(qt_module)

gcov {
    CONFIG += static
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
}

HEADERS += \
    qt3dquick_global.h \
    qt3dquick_global_p.h \
    qt3dquickvaluetypes_p.h \
    qqmlaspectengine.h \
    qqmlaspectengine_p.h

SOURCES += \
    qt3dquick_global.cpp \
    qt3dquickvaluetypes.cpp \
    qqmlaspectengine.cpp

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

# otherwise mingw headers do not declare common functions like ::strcasecmp
win32-g++*:QMAKE_CXXFLAGS_CXX11 = -std=gnu++0x

include(./items/items.pri)

