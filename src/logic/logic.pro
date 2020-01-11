TARGET     = Qt3DLogic
MODULE     = 3dlogic
DEFINES   += BUILD_QT3D_MODULE

QT         = core-private gui-private 3dcore 3dcore-private

gcov {
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
}

include(logic.pri)

load(qt_module)
