option(host_build)
load(qt_tool)

SOURCES = $$PWD/qgltf.cpp

config_assimp:!cross_compile {
    !contains(QT_CONFIG, no-pkg-config) {
        CONFIG += link_pkgconfig
        PKGCONFIG += assimp
    } else {
        LIBS += -lassimp
    }
} else {
    include(../../src/3rdparty/assimp/assimp.pri)
}
