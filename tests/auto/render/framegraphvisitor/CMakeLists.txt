# Generated from framegraphvisitor.pro.

#####################################################################
## tst_framegraphvisitor Test:
#####################################################################

qt_add_test(tst_framegraphvisitor
    SOURCES
        ../commons/testrenderer.cpp ../commons/testrenderer.h
        tst_framegraphvisitor.cpp
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

#### Keys ignored in scope 1:.:.:framegraphvisitor.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_framegraphvisitor CONDITION useCommonTestAspect
    SOURCES
        ../commons/testaspect.cpp ../commons/testaspect.h
)
