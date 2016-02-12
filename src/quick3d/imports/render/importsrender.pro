CXX_MODULE = qml
TARGET = quick3drenderplugin
TARGETPATH = Qt3D/Render

QT += core-private qml qml-private 3dcore 3drender 3dquick 3dquick-private 3dquickrender-private

HEADERS += \
    qt3dquick3drenderplugin.h

SOURCES += \
    qt3dquick3drenderplugin.cpp

OTHER_FILES += qmldir

include(./defaults/defaults.pri)

OTHER_FILES += \
    $$QML_FILES

# Create a resource file for qml files that need to be registered by the plugin
GENERATED_RESOURCE_FILE = $$OUT_PWD/defaults.qrc
INCLUDED_RESOURCE_FILES = $$QML_FILES
RESOURCE_CONTENT = \
    "<RCC>" \
    "<qresource prefix=\"/Qt3D/Render/\">"

for(resourcefile, INCLUDED_RESOURCE_FILES) {
    resourcefileabsolutepath = $$absolute_path($$resourcefile)
    relativepath_in = $$relative_path($$resourcefileabsolutepath, $$_PRO_FILE_PWD_)
    relativepath_out = $$relative_path($$resourcefileabsolutepath, $$OUT_PWD)
    RESOURCE_CONTENT += "<file alias=\"$$relativepath_in\">$$relativepath_out</file>"
}

RESOURCE_CONTENT += \
    "</qresource>" \
    "</RCC>"

write_file($$GENERATED_RESOURCE_FILE, RESOURCE_CONTENT)|error("Aborting.")

RESOURCES += $$GENERATED_RESOURCE_FILE

load(qml_plugin)
