# Generated from qboundingvolumedebug.pro.

#####################################################################
## tst_qboundingvolumedebug Test:
#####################################################################

qt_internal_add_test(tst_qboundingvolumedebug
    SOURCES
        ../commons/testrenderer.cpp ../commons/testrenderer.h
        tst_qboundingvolumedebug.cpp
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

#### Keys ignored in scope 1:.:.:qboundingvolumedebug.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_internal_extend_target(tst_qboundingvolumedebug CONDITION useCommonTestAspect
    SOURCES
        ../commons/testaspect.cpp ../commons/testaspect.h
)
