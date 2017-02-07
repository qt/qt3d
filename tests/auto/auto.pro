TEMPLATE = subdirs

SUBDIRS = \
    core \
    render \
    quick3d \
    cmake \
    input \
    animation \
    extras

QT_FOR_CONFIG += 3dcore-private
qtConfig(bullet) {
    SUBDIRS = physics
}

installed_cmake.depends = cmake
