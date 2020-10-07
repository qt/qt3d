# Generated from memorybarrier.pro.

#####################################################################
## tst_memorybarrier Test:
#####################################################################

qt_internal_add_test(tst_memorybarrier
    SOURCES
        tst_memorybarrier.cpp
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:memorybarrier.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../commons/commons.cmake)
qt3d_setup_common_render_test(tst_memorybarrier)
