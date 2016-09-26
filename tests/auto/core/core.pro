TEMPLATE = subdirs

SUBDIRS = \
    handle \
    handlemanager \
    arrayresourcesmanager \
    qcircularbuffer \
    qboundedcircularbuffer \
    nodes \
    qaspectengine \
    qaspectfactory \
    qaspectjob \
    qchangearbiter \
    qscene \
    qservicelocator \
    qjoint \
    qskeletonloader \
    qskeleton \
    qarmature

qtConfig(private_tests) {
    SUBDIRS += \
    qentity \
    qframeallocator \
    qtransform \
    threadpooler \
    qpostman \
    vector4d_base

    QT_FOR_CONFIG += 3dcore-private
    qtConfig(qt3d-profile-jobs): SUBDIRS += aspectcommanddebugger
    qtConfig(qt3d-simd-sse2): SUBDIRS += vector4d_sse
}
