TEMPLATE = subdirs

module_qt3d_src.subdir = src
module_qt3d_src.target = module-qt3d-src

module_qt3d_examples.subdir = examples
module_qt3d_examples.target = module-qt3d-examples
module_qt3d_examples.depends = module_qt3d_src
!contains(QT_BUILD_PARTS,examples) {
    module_qt3d_examples.CONFIG = no_default_target no_default_install
}

module_qt3d_demos.subdir = demos
module_qt3d_demos.target = module-qt3d-demos
module_qt3d_demos.depends = module_qt3d_src
!contains(QT_BUILD_PARTS,examples) {
    module_qt3d_demos.CONFIG = no_default_target no_default_install
}

module_qt3d_tutorials.subdir = tutorials
module_qt3d_tutorials.target = module-qt3d-tutorials
module_qt3d_tutorials.depends = module_qt3d_src
module_qt3d_tutorials.CONFIG = no_default_target no_default_install

module_qt3d_tests.subdir = tests
module_qt3d_tests.target = module-qt3d-tests
module_qt3d_tests.depends = module_qt3d_src
module_qt3d_tests.CONFIG = no_default_install
!contains(QT_BUILD_PARTS,tests):module_qt3d_tests.CONFIG += no_default_target

module_qt3d_util.subdir = util
module_qt3d_util.target = module-qt3d-util
module_qt3d_util.depends = module_qt3d_src
module_qt3d_util.CONFIG = no_default_target no_default_install

SUBDIRS += module_qt3d_src \
           module_qt3d_examples \
           module_qt3d_demos \
           module_qt3d_tests

!package: SUBDIRS += module_qt3d_tutorials

!gcov: SUBDIRS += module_qt3d_util

# We need opengl, minimum es2 or desktop
!contains(QT_CONFIG, opengl) {
    error(Qt3D requires OpenGL!)
}
contains(QT_CONFIG, opengles1) {
    error(Qt3D does not support OpenGL ES 1!)
}
