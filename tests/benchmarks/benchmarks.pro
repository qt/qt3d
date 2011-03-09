TEMPLATE = subdirs
SUBDIRS = qarray \
    qglbuilder_perf
contains(QT_CONFIG, declarative):SUBDIRS += matrix_properties
