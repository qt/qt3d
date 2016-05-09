TEMPLATE = subdirs

SUBDIRS += \
    simple-qml \
    simple-cpp \
    gltf \
    multiviewport \
    wireframe \
    tessellation-modes \
    deferred-renderer-qml \
    shadow-map-qml \
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
    mouseinput-qml \
    instanced-arrays-qml \
    picking-qml \
    transforms-qml \
    lights \
    compute-particles

qtHaveModule(multimedia): SUBDIRS += audio-visualizer-qml

# qmake seems to break in some CI configurations, disable this for now
#SUBDIRS += qgltf

# TODO Port the old examples to new APIs
#SUBDIRS += qt3d

qtHaveModule(widgets): SUBDIRS += assimp-cpp \
    basicshapes-cpp
