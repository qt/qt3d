TARGET = Qt3DBulletPhysics

QT += core 3dcore core-private 3dcore-private

LIBS += -lBulletDynamics -lBulletCollision -lLinearMath

DEFINES += QT3DBULLETPHYSICS_LIBRARY

MODULE = 3dbulletphysics

load(qt_module)

include(bulletphysics.pri)

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += bullet
}

gcov {
    CONFIG += static
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
} else {
    CONFIG += dll
}

# otherwise mingw headers do not declare common functions like ::strcasecmp
win32-g++*:QMAKE_CXXFLAGS_CXX11 = -std=gnu++0x

PUBLIC_HEADERS = $$HEADERS
HEADERS += $$PRIVATE_HEADERS \
    qt3dbulletphysics_global.h

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL
