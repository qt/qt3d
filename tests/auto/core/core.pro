TEMPLATE = subdirs

SUBDIRS = \
    handle \
    handlemanager \
    arrayresourcesmanager \
    qcircularbuffer \
    qboundedcircularbuffer \
    nodes \
    qentity \
    qaspectengine \
    qaspectfactory \
    qaspectjob \
    qchangearbiter \
    qscene \
    qservicelocator \
    qtransform

contains(QT_CONFIG, private_tests) {
    SUBDIRS += \
    qframeallocator \
    cloning \
    threadpooler
}
