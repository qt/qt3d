TARGET   = Qt3DQuickScene2D
MODULE   = 3dquickscene2d
DEFINES += BUILD_QT3D_MODULE

QT      += core core-private qml qml-private 3dcore 3drender 3dquick 3dquick-private 3dcore-private 3drender-private
CONFIG -= precompile_header

gcov {
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
}

SOURCES += \
    qt3dquickscene2d_global.cpp \
    qt3dquickscene2dnodefactory.cpp \
    qt3dquickscene2d_logging.cpp

HEADERS += \
    qt3dquickscene2dnodefactory_p.h \
    qt3dquickscene2d_global_p.h \
    qt3dquickscene2d_global.h \
    qt3dquickscene2d_logging_p.h

include(./items/items.pri)

load(qt_module)
