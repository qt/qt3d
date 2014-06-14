TEMPLATE=subdirs
CONFIG += ordered

qtHaveModule(3dcore) {
SUBDIRS+= \
    core \
    render \
    bulletphysics
}
