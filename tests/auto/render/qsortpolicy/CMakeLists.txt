# Generated from qsortpolicy.pro.

#####################################################################
## tst_qsortmethod Test:
#####################################################################

qt_internal_add_test(tst_qsortmethod
    SOURCES
        tst_qsortpolicy.cpp
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:qsortpolicy.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../../core/common/common.cmake)
qt3d_setup_common_test(tst_qsortmethod)
