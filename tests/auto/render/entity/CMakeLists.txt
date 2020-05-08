# Generated from entity.pro.

#####################################################################
## tst_entity Test:
#####################################################################

qt_add_test(tst_entity
    SOURCES
        ../commons/testrenderer.cpp ../commons/testrenderer.h
        tst_entity.cpp
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

#### Keys ignored in scope 1:.:.:entity.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_entity CONDITION useCommonTestAspect
    SOURCES
        ../commons/testaspect.cpp ../commons/testaspect.h
)
