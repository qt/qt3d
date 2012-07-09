CXX_MODULE = 3d
TARGET  = qthreedqmlplugin

QT += quick qml 3d 3dquick

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

QML_FILES += \
    library.xml

OTHER_FILES += \
    README.plugins_types \
    README.library_xml

load(qml_plugin)
