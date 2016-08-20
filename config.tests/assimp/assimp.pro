SOURCES += main.cpp

unix:qtConfig(pkg-config) {
    CONFIG += link_pkgconfig
    PKGCONFIG += assimp
} else {
    LIBS += -lassimp
}
