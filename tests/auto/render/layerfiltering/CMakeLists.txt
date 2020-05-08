# Generated from layerfiltering.pro.

#####################################################################
## tst_layerfiltering Test:
#####################################################################

qt_add_test(tst_layerfiltering
    SOURCES
        ../commons/testrenderer.cpp ../commons/testrenderer.h
        tst_layerfiltering.cpp
    INCLUDE_DIRECTORIES
        ../commons
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:layerfiltering.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_layerfiltering CONDITION useCommonTestAspect
    SOURCES
        ../commons/testaspect.cpp ../commons/testaspect.h
)
