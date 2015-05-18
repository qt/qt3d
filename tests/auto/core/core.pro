TEMPLATE = subdirs

SUBDIRS = \
    handle \
    handlemanager \
    arrayresourcesmanager \
    listresourcesmanager \
    qcircularbuffer \
    qboundedcircularbuffer \
    nodes \
    qentity \
    qaspectengine \
    qaspectfactory \
    qchangearbiter \
    qscene \
    qservicelocator \
    qray3d

contains(QT_CONFIG, private_tests) {
    SUBDIRS += \
    qframeallocator \
    cloning \
    threadpooler
}
