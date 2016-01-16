config_assimp:!if(cross_compile:host_build) {
    unix:!contains(QT_CONFIG, no-pkg-config) {
        CONFIG += link_pkgconfig
        PKGCONFIG_PRIVATE += assimp
    } else {
        LIBS += -lassimp
    }
    return()
} else {
    include(assimp.pri)
}
