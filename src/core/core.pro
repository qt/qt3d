TARGET     = Qt3DCore
MODULE     = 3dcore

QT         = core-private gui-private

# Qt3D is free of Q_FOREACH - make sure it stays that way:
DEFINES += QT_NO_FOREACH

gcov {
    CONFIG += static
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
}

# otherwise mingw headers do not declare common functions like ::strcasecmp
win32-g++*:QMAKE_CXXFLAGS_CXX11 = -std=gnu++0x

include(core.pri)
include(aspects/aspects.pri)
include(changes/changes.pri)
include(jobs/jobs.pri)
include(nodes/nodes.pri)
include(transforms/transforms.pri)
include(resources/resources.pri)
include(services/services.pri)

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

load(qt_module)
