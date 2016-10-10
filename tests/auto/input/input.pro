TEMPLATE = subdirs

contains(QT_CONFIG, private_tests) {
    SUBDIRS += \
        qaxis \
        qaction \
        qactioninput \
        qabstractaxisinput \
        qanalogaxisinput \
        qbuttonaxisinput \
        qkeyboardhandler \
        qlogicaldevice \
        axis \
        action \
        abstractaxisinput \
        actioninput \
        analogaxisinput \
        buttonaxisinput \
        inputsequence \
        inputchord \
        keyboardhandler \
        qabstractphysicaldevicebackendnode
}
