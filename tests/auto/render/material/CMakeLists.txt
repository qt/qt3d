# Generated from material.pro.

# special case skip regeneration

#####################################################################
## tst_material Test:
#####################################################################

qt_internal_add_test(tst_qt3d_material
    SOURCES
        tst_material.cpp
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:material.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../commons/commons.cmake)
qt3d_setup_common_render_test(tst_qt3d_material)
