# Generated from picking-qml.pro.

#####################################################################
## picking-qml Binary:
#####################################################################

qt_internal_add_manual_test(picking-qml
    GUI
    SOURCES
        main.cpp
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DInput
        Qt::3DQuick
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(picking-qml_resource_files
    "PickableEntity.qml"
    "main.qml"
)

qt_internal_add_resource(picking-qml "picking-qml"
    PREFIX
        "/"
    FILES
        ${picking-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:picking-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml"

## Scopes:
#####################################################################
