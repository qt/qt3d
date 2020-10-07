# Generated from qtextureimagedata.pro.

#####################################################################
## tst_qtextureimagedata Test:
#####################################################################

qt_internal_add_test(tst_qtextureimagedata
    SOURCES
        tst_qtextureimagedata.cpp
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:qtextureimagedata.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../../core/common/common.cmake)
qt3d_setup_common_test(tst_qtextureimagedata)
