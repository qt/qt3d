# Generated from qmemorybarrier.pro.

#####################################################################
## tst_qmemorybarrier Test:
#####################################################################

qt_internal_add_test(tst_qmemorybarrier
    SOURCES
        tst_qmemorybarrier.cpp
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:qmemorybarrier.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../commons/commons.cmake)
qt3d_setup_common_render_test(tst_qmemorybarrier)
