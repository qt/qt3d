# Generated from raycasting-qml.pro.

#####################################################################
## raycasting-qml Binary:
#####################################################################

qt_add_manual_test(raycasting-qml
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
set(raycasting-qml_resource_files
    "main.qml"
)

qt_add_resource(raycasting-qml "raycasting-qml"
    PREFIX
        "/"
    FILES
        ${raycasting-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:raycasting-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml"

## Scopes:
#####################################################################
