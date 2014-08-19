SOURCES += main.cpp

LIBS += -lBulletDynamics -lBulletCollision -lLinearMath

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += bullet
}
