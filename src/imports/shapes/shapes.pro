TEMPLATE = lib
TARGET  = qshapesqmlplugin
CONFIG += qt plugin

QT += qt3d quick3d

DESTDIR = $$QT.quick3d.imports/Qt3D
target.path = $$[QT_INSTALL_IMPORTS]/Qt3D
INSTALLS += target

SOURCES += \
    sphere.cpp \
    shapes.cpp \
    cylinder.cpp \
    capsule.cpp \
    line.cpp \
    point.cpp
HEADERS += \
    sphere.h \
    cylinder.h \
    capsule.h \
    line.h \
    point.h

qdeclarativesources.files += \
    qmldir \
    library.xml \
    plugins.qmltypes \
    Cube.qml \
    cube.obj \
    Quad.qml \
    quad.obj \
    Teapot.qml \
    teapot.bez
# see the file README.library_xml for more on library.xml

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt3D
INSTALLS += qdeclarativesources

OTHER_FILES += \
    README.plugins_types \
    README.library_xml
