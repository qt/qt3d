TEMPLATE = subdirs

contains(QT_CONFIG, private_tests) {
    SUBDIRS += \
        qaxis \
        qaction \
        qaxisinput \
        qactioninput \
        qlogicaldevice \
        axis \
        action \
        axisinput
}
