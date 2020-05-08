# Generated from qcameraselector.pro.

#####################################################################
## tst_qcameraselector Test:
#####################################################################

qt_add_test(tst_qcameraselector
    SOURCES
        tst_qcameraselector.cpp
    INCLUDE_DIRECTORIES
        ../../core/common
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:qcameraselector.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_qcameraselector CONDITION QT_FEATURE_private_tests
    SOURCES
        ../../core/common/qbackendnodetester.cpp ../../core/common/qbackendnodetester.h
        ../../core/common/testarbiter.h
)
