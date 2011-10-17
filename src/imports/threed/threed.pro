TEMPLATE = lib
TARGET  = qthreedqmlplugin
CONFIG += qt plugin

QT += qt3d quick3d

DESTDIR = $$QT.quick3d.imports/Qt3D
target.path = $$[QT_INSTALL_IMPORTS]/Qt3D
INSTALLS += target

SOURCES += \
    threed.cpp \
    qt3dnamespace.cpp \
    scale3d.cpp \
    viewport.cpp \
    qgraphicslookattransform.cpp \
    shaderprogram.cpp \
    skybox.cpp \
    billboarditem3d.cpp

HEADERS += \
    qt3dnamespace.h \
    scale3d.h \
    viewport.h \
    qgraphicslookattransform.h \
    shaderprogram.h \
    shaderprogram_p.h \
    skybox.h \
    billboarditem3d.h

# See the file README.library_xml for more on this
qdeclarativesources.files += \
    qmldir \
    library.xml \
    plugins.qmltypes

qdeclarativesources.path += $$[QT_INSTALL_IMPORTS]/Qt3D
INSTALLS += qdeclarativesources



OTHER_FILES += \
    README.plugins_types \
    README.library_xml


