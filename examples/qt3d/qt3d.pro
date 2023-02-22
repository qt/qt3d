TEMPLATE = subdirs

SUBDIRS += \
    simple-cpp \
    3d-text

# qmake seems to break in some CI configurations, disable this for now
#SUBDIRS += qgltf

# TODO Port the old examples to new APIs
#SUBDIRS += qt3d

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
