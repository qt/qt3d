CXX_MODULE = qml
TARGET = quick3drendererplugin
TARGETPATH = Qt3D/Renderer

QT += core-private qml qml-private 3dcore 3drenderer 3dquick 3dquickrenderer-private

HEADERS += \
    qt3dquick3drendererplugin.h

SOURCES += \
    qt3dquick3drendererplugin.cpp

OTHER_FILES += qmldir

load(qml_plugin)

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
