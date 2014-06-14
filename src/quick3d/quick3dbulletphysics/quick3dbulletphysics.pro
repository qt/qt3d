TARGET   = Qt3DQuickBulletPhysics

QT      += core core-private qml quick 3dcore 3dbulletphysics 3dquick

DEFINES += QT3DQUICKBULLETPHYSICS_LIBRARY

MODULE   = 3dquickbulletphysics

load(qt_module)

gcov {
    CONFIG += static
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
} else {
    CONFIG += dll
}

HEADERS += $$PRIVATE_HEADERS \
    qt3dquickbulletphysics_global.h

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

# otherwise mingw headers do not declare common functions like ::strcasecmp
win32-g++*:QMAKE_CXXFLAGS_CXX11 = -std=gnu++0x

include(./items/items.pri)

