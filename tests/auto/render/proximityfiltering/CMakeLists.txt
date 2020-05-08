# Generated from proximityfiltering.pro.

#####################################################################
## tst_proximityfiltering Test:
#####################################################################

qt_add_test(tst_proximityfiltering
    SOURCES
        ../commons/testrenderer.cpp ../commons/testrenderer.h
        tst_proximityfiltering.cpp
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

#### Keys ignored in scope 1:.:.:proximityfiltering.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_proximityfiltering CONDITION useCommonTestAspect
    SOURCES
        ../commons/testaspect.cpp ../commons/testaspect.h
)
