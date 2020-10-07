# Generated from transforms-qml.pro.

#####################################################################
## transforms-qml Binary:
#####################################################################

qt_internal_add_manual_test(transforms-qml
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
set(transforms-qml_resource_files
    "RenderableEntity.qml"
    "TemporaryCamera.qml"
    "TrefoilKnot.qml"
    "main.qml"
)

qt_internal_add_resource(transforms-qml "transforms-qml"
    PREFIX
        "/"
    FILES
        ${transforms-qml_resource_files}
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/obj/ball.obj"
    PROPERTIES QT_RESOURCE_ALIAS "assets/obj/ball.obj"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/obj/material-sphere.obj"
    PROPERTIES QT_RESOURCE_ALIAS "assets/obj/material-sphere.obj"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/obj/plane-10x10.obj"
    PROPERTIES QT_RESOURCE_ALIAS "assets/obj/plane-10x10.obj"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/obj/toyplane.obj"
    PROPERTIES QT_RESOURCE_ALIAS "assets/obj/toyplane.obj"
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/obj/trefoil.obj"
    PROPERTIES QT_RESOURCE_ALIAS "assets/obj/trefoil.obj"
)
set(obj_resource_files
    "assets/obj/ball.obj"
    "assets/obj/material-sphere.obj"
    "assets/obj/plane-10x10.obj"
    "assets/obj/toyplane.obj"
    "assets/obj/trefoil.obj"
)

qt_internal_add_resource(transforms-qml "obj"
    PREFIX
        "/"
    BASE
        "../../../examples/qt3d/exampleresources"
    FILES
        ${obj_resource_files}
)


#### Keys ignored in scope 1:.:.:transforms-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml" "RenderableEntity.qml" "TemporaryCamera.qml" "TrefoilKnot.qml"

## Scopes:
#####################################################################
