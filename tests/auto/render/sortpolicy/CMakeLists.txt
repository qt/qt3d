# Generated from sortpolicy.pro.

#####################################################################
## tst_sortpolicy Test:
#####################################################################

qt_internal_add_test(tst_sortpolicy
    SOURCES
        tst_sortpolicy.cpp
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:sortpolicy.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../commons/commons.cmake)
qt3d_setup_common_render_test(tst_sortpolicy)
