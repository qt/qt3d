# Generated from blendedclipanimator.pro.

#####################################################################
## tst_blendedclipanimator Test:
#####################################################################

qt_internal_add_test(tst_blendedclipanimator
    SOURCES
        tst_blendedclipanimator.cpp
    PUBLIC_LIBRARIES
        Qt::3DAnimation
        Qt::3DAnimationPrivate
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:blendedclipanimator.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../../core/common/common.cmake)
qt3d_setup_common_test(tst_blendedclipanimator)
