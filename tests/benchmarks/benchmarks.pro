TEMPLATE = subdirs
SUBDIRS = \
    qarray \
    qglbuilder_perf
qtHaveModule(qml): SUBDIRS += matrix_properties
