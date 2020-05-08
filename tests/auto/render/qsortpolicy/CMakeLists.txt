# Generated from qsortpolicy.pro.

#####################################################################
## tst_qsortmethod Test:
#####################################################################

qt_add_test(tst_qsortmethod
    SOURCES
        tst_qsortpolicy.cpp
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

#### Keys ignored in scope 1:.:.:qsortpolicy.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_qsortmethod CONDITION QT_FEATURE_private_tests
    SOURCES
        ../../core/common/qbackendnodetester.cpp ../../core/common/qbackendnodetester.h
        ../../core/common/testarbiter.h
)
