TEMPLATE = subdirs

qtConfig(private_tests) {
    SUBDIRS += \
        qshadergenerator \
        qshadergraph \
        qshadergraphloader \
        qshadernodes \
        qshadernodesloader
}
