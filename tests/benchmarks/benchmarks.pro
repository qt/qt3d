TEMPLATE = subdirs
SUBDIRS = \
    qarray \
    qglbuilder_perf
!isEmpty(QT.qml.name): SUBDIRS += matrix_properties
