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
    qtHaveModule(quickwidgets): SUBDIRS += widgets-scene3d
}

qtHaveModule(quick) {
    qtHaveModule(multimedia) {
#        SUBDIRS += audio-visualizer-qml
    }

    SUBDIRS += \
        simple-qml \
        shadow-map-qml \
        instanced-arrays-qml \
        planets-qml \
        advancedcustommaterial \
        anaglyph-rendering \
        compute-particles \
        phong-cubes \
        lights \
        controlsunderlay \
        simplecustommaterial \
        qardboard \
        pbr-materials \
#        scene2d \ # Restore once Scene2D has been ported to Qt6
        multiviewport \
        wireframe \
        wave \
        scene3d \
        controls
}

EXAMPLE_FILES += \
    exampleresources
