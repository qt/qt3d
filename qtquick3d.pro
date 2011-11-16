TEMPLATE = subdirs
SUBDIRS += src
!gcov: SUBDIRS += util examples demos

include(pkg.pri)

!package: SUBDIRS += tutorials

tests.CONFIG = no_default_install
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

include(pkg.pri)

# Install qt3d.prf into the Qt mkspecs so that "CONFIG += qt3d"
# can be used in applications to build against Qt3D.
package {
    distInstalls.files = features/qt3d.prf features/qt3dquick.prf
    distInstalls.path = $$QT3D_INSTALL_DATA/mkspecs/features
    !qtc_hmtn: INSTALLS += distInstalls
} else {
    win32 {
        Qt3DFile=$$PWD\\features\\qt3d.prf
        Qt3DFile=$$replace(Qt3DFile,/,\\)
        featuresDir=$$QT3D_INSTALL_DATA\\mkspecs\\features
        featuresDir=$$replace(featuresDir,/,\\)
        system(copy "$$Qt3DFile $$featuresDir")

        Qt3DQuickFile=$$PWD\\features\\qt3dquick.prf
        Qt3DQuickFile=$$replace(Qt3DQuickFile,/,\\)
        system(copy "$$Qt3DQuickFile $$featuresDir")
    } else {
        Qt3DFile=$$PWD/features/qt3d.prf
        featuresDir=$$QT3D_INSTALL_DATA/mkspecs/features
        system(cp "$$Qt3DFile $$featuresDir")

        Qt3DQuickFile=$$PWD/features/qt3dquick.prf
        system(cp "$$Qt3DQuickFile $$featuresDir")
    }
}

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog
