#requires(qtHaveModule(opengl))

load(configure)
qtCompileTest(assimp)
load(qt_parts)

#module_qt3d_tutorials.subdir = tutorials
#module_qt3d_tutorials.target = sub-tutorials
#module_qt3d_tutorials.depends = sub_src
#module_qt3d_tutorials.CONFIG = no_default_target no_default_install

#!package: SUBDIRS += module_qt3d_tutorials

#gcov: SUBDIRS -= sub_tools

# We need opengl, minimum es2
contains(QT_CONFIG, opengles1) {
    error("Qt3D does not support OpenGL ES 1!")
}

OTHER_FILES += \
    sync.profile
