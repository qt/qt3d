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
    planets-qml \
    plasma \
    gooch-qml \
    transparency-qml \
    transparency-qml-scene3d \
    scene3d-loader \
    clip-planes-qml \
    mouseinput-qml \
    custom-mesh-cpp \
    bigscene-instanced-qml \
    custom-mesh-qml \
    instanced-arrays-qml \
    picking-qml \
    transforms-qml \
    lights

qtHaveModule(multimedia): SUBDIRS += audio-visualizer-qml

# qmake seems to break in some CI configurations, disable this for now
#SUBDIRS += qgltf

# TODO Port the old examples to new APIs
#SUBDIRS += qt3d
qtHaveModule(widgets): SUBDIRS += assimp-cpp \
    basicshapes-cpp

EXAMPLE_FILES += \
    common \
    exampleresources
