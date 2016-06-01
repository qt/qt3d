TEMPLATE = subdirs

contains(QT_CONFIG, private_tests) {
    SUBDIRS += \
        quick3dnodeinstantiator \
        dynamicnodecreation
}
