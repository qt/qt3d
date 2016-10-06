TEMPLATE = subdirs

qtConfig(private_tests) {
    SUBDIRS += \
        quick3dnodeinstantiator \
        dynamicnodecreation \
        3drender
}
