TEMPLATE = subdirs

contains(QT_CONFIG, private_tests) {
    SUBDIRS += \
        qaxis \
        qaction \
        qactioninput \
        qaxisinput \
        qanalogaxisinput \
        qbuttonaxisinput \
        qlogicaldevice \
        axis \
        action \
        axisinput \
        analogaxisinput \
        buttonaxisinput
}
