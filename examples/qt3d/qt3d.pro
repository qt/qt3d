TEMPLATE = subdirs

SUBDIRS += \
    playground-qml \
    simple-qml \
    simple-cpp \
    gltf \
    assimp \
    cpp_example \
    multiviewport \
    torus-qml \
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
    keyboardinput-qml \
    loader-qml \
    wave \
    materials-cpp \
    dynamicscene-cpp \
    scene3d \
    enabled-qml \
    skybox \
    controls \
    anaglyph-rendering \
    simple-shaders-qml \
    planets-qml

# TODO Port the old examples to new APIs
#SUBDIRS += qt3d
qtHaveModule(widgets): SUBDIRS += assimp-cpp \
    basicshapes-cpp
