# Generated from qtechniquefilter.pro.

#####################################################################
## tst_qtechniquefilter Test:
#####################################################################

qt_internal_add_test(tst_qtechniquefilter
    SOURCES
        tst_qtechniquefilter.cpp
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:qtechniquefilter.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../../core/common/common.cmake)
qt3d_setup_common_test(tst_qtechniquefilter)
