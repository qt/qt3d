# Generated from channelmapper.pro.

#####################################################################
## tst_channelmapper Test:
#####################################################################

qt_internal_add_test(tst_channelmapper
    SOURCES
        tst_channelmapper.cpp
    PUBLIC_LIBRARIES
        Qt::3DAnimation
        Qt::3DAnimationPrivate
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:channelmapper.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../../core/common/common.cmake)
qt3d_setup_common_test(tst_channelmapper)
