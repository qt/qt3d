TARGET   = Qt3DQuickInput
MODULE   = 3dquickinput
DEFINES += BUILD_QT3D_MODULE

QT      += core core-private qml qml-private 3dcore 3dinput 3dquick 3dquick-private 3dcore-private 3dinput-private
CONFIG -= precompile_header

gcov {
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

include(./items/items.pri)

load(qt_module)
