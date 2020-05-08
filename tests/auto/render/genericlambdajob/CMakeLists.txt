# Generated from genericlambdajob.pro.

#####################################################################
## tst_genericlambdajob Test:
#####################################################################

qt_add_test(tst_genericlambdajob
    SOURCES
        ../commons/testrenderer.cpp ../commons/testrenderer.h
        tst_genericlambdajob.cpp
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

#### Keys ignored in scope 1:.:.:genericlambdajob.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_genericlambdajob CONDITION useCommonTestAspect
    SOURCES
        ../commons/testaspect.cpp ../commons/testaspect.h
)
