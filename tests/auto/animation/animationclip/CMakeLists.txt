# Generated from animationclip.pro.

#####################################################################
## tst_animationclip Test:
#####################################################################

qt_internal_add_test(tst_animationclip
    SOURCES
        tst_animationclip.cpp
    PUBLIC_LIBRARIES
        Qt::3DAnimation
        Qt::3DAnimationPrivate
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:animationclip.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../../core/common/common.cmake)
qt3d_setup_common_test(tst_animationclip)
