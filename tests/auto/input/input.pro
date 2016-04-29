TEMPLATE = subdirs

contains(QT_CONFIG, private_tests) {
    SUBDIRS += \
        qaxis \
        qaction \
        qactioninput \
        qaxisinput \
        qbuttonaxisinput \
        qlogicaldevice \
        axis \
        action \
        axisinput \
        buttonaxisinput
}
