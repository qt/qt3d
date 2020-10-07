# Generated from channelmapping.pro.

#####################################################################
## tst_channelmapping Test:
#####################################################################

qt_internal_add_test(tst_channelmapping
    SOURCES
        tst_channelmapping.cpp
    INCLUDE_DIRECTORIES
        ../../core/common
    PUBLIC_LIBRARIES
        Qt::3DAnimation
        Qt::3DAnimationPrivate
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:channelmapping.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../../core/common/common.cmake)
qt3d_setup_common_test(tst_channelmapping)
