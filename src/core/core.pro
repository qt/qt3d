TARGET = Qt3DCore
MODULE = 3dcore

QT = core-private gui-private network
QT_FOR_PRIVATE = concurrent

gcov {
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
}

include(core.pri)
include(aspects/aspects.pri)
include(geometry/geometry.pri)
include(jobs/jobs.pri)
include(nodes/nodes.pri)
include(transforms/transforms.pri)
include(resources/resources.pri)
include(services/services.pri)
include(aspect/aspect.pri)

load(qt_module)
