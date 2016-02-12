TARGET     = Qt3DLogic
MODULE     = 3dlogic

QT         = core-private gui-private 3dcore 3dcore-private

gcov {
    CONFIG += static
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
}

# otherwise mingw headers do not declare common functions like ::strcasecmp
win32-g++*:QMAKE_CXXFLAGS_CXX11 = -std=gnu++0x

include(logic.pri)

load(qt_module)
