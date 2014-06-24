TEMPLATE = subdirs

SUBDIRS = \
    handle \
    handlemanager \
    arrayresourcesmanager \
    listresourcesmanager \
    qcircularbuffer \
    qboundedcircularbuffer \
    nodes

contains(QT_CONFIG, private_tests) {
    SUBDIRS += qframeallocator
}
