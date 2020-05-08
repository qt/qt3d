# Generated from assimp.pro.

#####################################################################
## assimp Binary:
#####################################################################

qt_add_manual_test(assimp
    GUI
    SOURCES
        main.cpp
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DQuick
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(assimp_resource_files
    "main.qml"
)

qt_add_resource(assimp "assimp"
    PREFIX
        "/"
    FILES
        ${assimp_resource_files}
)
set_source_files_properties("../../../examples/qt3d/exampleresources/assets/test_scene.dae"
    PROPERTIES QT_RESOURCE_ALIAS "assets/test_scene.dae"
)
set(test_scene_resource_files
    "assets/test_scene.dae"
)

qt_add_resource(assimp "test_scene"
    PREFIX
        "/"
    BASE
        "../../../examples/qt3d/exampleresources"
    FILES
        ${test_scene_resource_files}
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

qt_add_resource(assimp "obj"
    PREFIX
        "/"
    BASE
        "../../../examples/qt3d/exampleresources"
    FILES
        ${obj_resource_files}
)


#### Keys ignored in scope 1:.:.:assimp.pro:<TRUE>:
# OTHER_FILES = "main.qml"
# SOURCE = "main.cpp"

## Scopes:
#####################################################################
