TEMPLATE = subdirs

SUBDIRS += \
    simple-qml \
    simple-cpp \
    multiviewport \
    wireframe \
    shadow-map-qml \
    materials \
    wave \
    materials-cpp \
    scene3d \
    controls \
    anaglyph-rendering \
    planets-qml \
    transparency-qml \
    transparency-qml-scene3d \
    instanced-arrays-qml \
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
