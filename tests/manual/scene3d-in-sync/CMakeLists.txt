# Generated from scene3d-in-sync.pro.

#####################################################################
## scene3d-in-sync Binary:
#####################################################################

qt_internal_add_manual_test(scene3d-in-sync
    GUI
    SOURCES
        main.cpp
    PUBLIC_LIBRARIES
        Qt::3DInput
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(scene3d-in-sync_resource_files
    "main.qml"
)

qt_internal_add_resource(scene3d-in-sync "scene3d-in-sync"
    PREFIX
        "/"
    FILES
        ${scene3d-in-sync_resource_files}
)


#### Keys ignored in scope 1:.:.:scene3d-in-sync.pro:<TRUE>:
# OTHER_FILES = "main.qml"
