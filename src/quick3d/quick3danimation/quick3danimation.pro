TARGET   = Qt3DQuickAnimation
MODULE   = 3dquickanimation
DEFINES += BUILD_QT3D_MODULE

QT      += core core-private qml qml-private 3dcore 3drender 3danimation 3dquick 3dquick-private 3dcore-private 3drender-private
CONFIG -= precompile_header

gcov {
    CONFIG += static
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
}

SOURCES += \
    qt3dquickanimation_global.cpp \
    qt3dquickanimationnodefactory.cpp

HEADERS += \
    qt3dquickanimationnodefactory_p.h \
    qt3dquickanimation_global_p.h \
    qt3dquickanimation_global.h

include(./items/items.pri)

load(qt_module)
