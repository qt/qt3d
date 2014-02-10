TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += \
    exampleresources \
    simple-qml \
    gltf

# TODO Port the old examples to new APIs
#SUBDIRS += qt3d
#qtHaveModule(qml): SUBDIRS += quick3d
