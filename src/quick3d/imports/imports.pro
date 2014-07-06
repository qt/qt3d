TEMPLATE=subdirs
CONFIG += ordered

qtHaveModule(3dcore) {
SUBDIRS+= \
    core \
    render
}

qtHaveModule(3dbulletphysics): SUBDIRS += bulletphysics
