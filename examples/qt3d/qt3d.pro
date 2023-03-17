TEMPLATE = subdirs

SUBDIRS += \
    simple-cpp \
    3d-text


qtHaveModule(widgets) {
    SUBDIRS += basicshapes-cpp
}

qtHaveModule(quick) {
    SUBDIRS += \
        simple-qml \
        simplecustommaterial \
        multiviewport \
        controls
}

EXAMPLE_FILES += \
    exampleresources
