# Generated from multiplewindows-qml.pro.

#####################################################################
## multiplewindows-qml Binary:
#####################################################################

qt_internal_add_manual_test(multiplewindows-qml
    GUI
    SOURCES
        main.cpp
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DInput
        Qt::3DQuick
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(qml_resource_files
    "Scene.qml"
    "main.qml"
)

qt_internal_add_resource(multiplewindows-qml "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)


#### Keys ignored in scope 1:.:.:multiplewindows-qml.pro:<TRUE>:
# DISTFILES = "Scene.qml"
# TEMPLATE = "app"

## Scopes:
#####################################################################
