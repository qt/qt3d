# Generated from quick3dnode.pro.

#####################################################################
## tst_quick3dnode Test:
#####################################################################

qt_add_test(tst_quick3dnode
    SOURCES
        ../../render/qmlscenereader/qmlscenereader.cpp ../../render/qmlscenereader/qmlscenereader.h
        tst_quick3dnode.cpp
    INCLUDE_DIRECTORIES
        ../../render/qmlscenereader
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DQuick
        Qt::3DQuickPrivate
        Qt::3DQuickRenderPrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
        Qt::Qml
)

# Resources:
set(quick3dnode_resource_files
    "quick3dnodev9.qml"
)

qt_add_resource(tst_quick3dnode "quick3dnode"
    PREFIX
        "/"
    FILES
        ${quick3dnode_resource_files}
)


#### Keys ignored in scope 1:.:.:quick3dnode.pro:<TRUE>:
# TEMPLATE = "app"
