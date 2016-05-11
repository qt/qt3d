TARGET   = Qt3DQuickInput
MODULE   = 3dquickinput

QT      += core core-private qml qml-private 3dcore 3dinput 3dquick 3dquick-private 3dcore-private 3dinput-private
CONFIG -= precompile_header

# Qt3D is free of Q_FOREACH - make sure it stays that way:
DEFINES += QT_NO_FOREACH

gcov {
    CONFIG += static
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
}

SOURCES += \
    qt3dquickinput_global.cpp \
    qt3dquickinputnodefactory.cpp


HEADERS += \
    qt3dquickinput_global_p.h \
    qt3dquickinput_global.h \
    qt3dquickinputnodefactory_p.h

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

# otherwise mingw headers do not declare common functions like ::strcasecmp
win32-g++*:QMAKE_CXXFLAGS_CXX11 = -std=gnu++0x

include(./items/items.pri)

load(qt_module)
