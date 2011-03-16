TEMPLATE = subdirs
SUBDIRS += src
!gcov: SUBDIRS += util examples demos
SUBDIRS += tests
CONFIG += ordered

include(doc/doc.pri)

# We need opengl, minimum es2 or desktop
!contains(QT_CONFIG, opengl) {
    error(QtQuick3D requires OpenGL!)
}
contains(QT_CONFIG, opengles1) {
    error(QtQuick3D does not support OpenGL ES 1!)
}
# We need qt declarative
!contains(QT_CONFIG, declarative) {
    error(QtQuick3D requires Qt Declarative!)
}

# Install qt3d.prf into the Qt mkspecs so that "CONFIG += qt3d"
# can be used in applications to build against Qt3D.
symbian|win32 {
    Qt3DFile=$$PWD\\features\\qt3d.prf
    Qt3DFile=$$replace(Qt3DFile,/,\\)
    featuresDir=$$[QT_INSTALL_DATA]\\mkspecs\\features
    featuresDir=$$replace(featuresDir,/,\\)
    system(copy "$$Qt3DFile $$featuresDir")

    Qt3DQuickFile=$$PWD\\features\\qt3dquick.prf
    Qt3DQuickFile=$$replace(Qt3DQuickFile,/,\\)
    system(copy "$$Qt3DQuickFile $$featuresDir")
} else {
    Qt3DFile=$$PWD/features/qt3d.prf
    featuresDir=$$[QT_INSTALL_DATA]/mkspecs/features
    system(cp "$$Qt3DFile $$featuresDir")

    Qt3DQuickFile=$$PWD/features/qt3dquick.prf
    system(cp "$$Qt3DQuickFile $$featuresDir")
}

symbian {
    # symbian needs to be at the end, because qt3d.pro does an ordered build,
    # and symbian depends on all the others.
    SUBDIRS += symbianinstall

    symbianinstall.subdir = devices/symbian
    symbianinstall.target = sub-symbianinstall
    symbianinstall.depends = $$SUBDIRS
    symbianinstall.depends -= symbianinstall
}
