TEMPLATE = subdirs
SUBDIRS += src
!gcov: SUBDIRS += util examples demos

!package: SUBDIRS += tutorials

tests.CONFIG = no_default_install
SUBDIRS += tests
CONFIG += ordered

include(doc/doc.pri)

# We need opengl, minimum es2 or desktop
!contains(QT_CONFIG, opengl) {
    error(Qt3D requires OpenGL!)
}
contains(QT_CONFIG, opengles1) {
    error(Qt3D does not support OpenGL ES 1!)
}
