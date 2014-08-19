SOURCES += main.cpp

LIBS += -lassimp

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += assimp
}
