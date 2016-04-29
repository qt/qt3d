TEMPLATE = subdirs

contains(QT_CONFIG, private_tests) {
    SUBDIRS += \
        qaxis \
        qaction \
        qactioninput \
        qabstractaxisinput \
        qanalogaxisinput \
        qbuttonaxisinput \
        qlogicaldevice \
        axis \
        action \
        abstractaxisinput \
        analogaxisinput \
        buttonaxisinput
}
