TEMPLATE = subdirs

SUBDIRS = \
    handle \
    qresourcemanager \
    qcircularbuffer \
    qboundedcircularbuffer \
    nodes \
    qaspectengine \
    qaspectfactory \
    qaspectjob \
    qchangearbiter \
    qscene \
    qservicelocator

qtConfig(private_tests) {
    SUBDIRS += \
    qentity \
    qframeallocator \
    qtransform \
    threadpooler \
    aspectcommanddebugger \
    qpostman
}
