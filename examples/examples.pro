TEMPLATE = subdirs

SUBDIRS += \
    exampleresources \
    playground-qml \
    simple-qml \
    simple-cpp \
    gltf \
    assimp \
    cpp_example \
    multiviewport \
    torus-qml \
    torus-cpp \
    cylinder-qml \
    cylinder-cpp \
    bigmodel-qml \
    wireframe \
    tessellation-modes \
    deferred-renderer-qml \
    deferred-renderer-cpp \
    shadow-map-qml \
    bigscene-cpp \
    materials \
    keyboardinput-qml

# TODO Port the old examples to new APIs
#SUBDIRS += qt3d
#qtHaveModule(qml): SUBDIRS += quick3d

qtHaveModule(3dbulletphysics) {
    SUBDIRS += rollerball
}

# Make all other subdirs depend on exampleresources
for(subdir, SUBDIRS) {
    !equals(subdir, exampleresources) {
        $${subdir}.depends += exampleresources
    }
}

