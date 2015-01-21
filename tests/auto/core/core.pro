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

contains(QT_CONFIG, private_tests) {
    SUBDIRS += \
    qframeallocator \
    cloning
}
