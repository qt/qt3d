SOURCES += main.cpp

unix:!contains(QT_CONFIG, no-pkg-config) {
    CONFIG += link_pkgconfig
    PKGCONFIG += assimp
} else {
    LIBS += -lassimp
}
