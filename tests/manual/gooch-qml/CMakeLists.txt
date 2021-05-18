# Generated from gooch-qml.pro.

#####################################################################
## gooch-qml Binary:
#####################################################################

qt_internal_add_manual_test(gooch-qml
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
set(gooch-qml_resource_files
    "MyEntity.qml"
    "main.qml"
)

qt_internal_add_resource(gooch-qml "gooch-qml"
    PREFIX
        "/"
    FILES
        ${gooch-qml_resource_files}
)

set(obj_resource_files
    "../../../examples/qt3d/exampleresources/assets/obj/ball.obj"
    "../../../examples/qt3d/exampleresources/assets/obj/material-sphere.obj"
    "../../../examples/qt3d/exampleresources/assets/obj/plane-10x10.obj"
    "../../../examples/qt3d/exampleresources/assets/obj/toyplane.obj"
    "../../../examples/qt3d/exampleresources/assets/obj/trefoil.obj"
)

qt_internal_add_resource(gooch-qml "obj"
    PREFIX
        "/"
    BASE
        "../../../examples/qt3d/exampleresources"
    FILES
        ${obj_resource_files}
)


#### Keys ignored in scope 1:.:.:gooch-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml" "MyEntity.qml"

## Scopes:
#####################################################################
