
include (renderer/renderer.pri)
include (jobs/jobs.pri)
include (io/io.pri)
include (textures/textures.pri)
include (graphicshelpers/graphicshelpers.pri)
include (renderstates/renderstates.pri)

gcov {
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
}
