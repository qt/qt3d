# Generated from default.pro.

#####################################################################
## DefaultGeometryLoaderPlugin Plugin:
#####################################################################

qt_internal_add_plugin(DefaultGeometryLoaderPlugin
    OUTPUT_NAME defaultgeometryloader
    TYPE geometryloaders
    SOURCES
        basegeometryloader.cpp basegeometryloader_p.h
        main.cpp
        objgeometryloader.cpp
        plygeometryloader.cpp
        stlgeometryloader.cpp
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Core
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:default.pro:<TRUE>:
# DISTFILES = "default.json"
