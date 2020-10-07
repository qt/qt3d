# Generated from qchangearbiter.pro.

#####################################################################
## tst_qchangearbiter Test:
#####################################################################

qt_internal_add_test(tst_qchangearbiter
    SOURCES
        tst_qchangearbiter.cpp
    INCLUDE_DIRECTORIES
        ../common
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:qchangearbiter.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_internal_extend_target(tst_qchangearbiter CONDITION QT_FEATURE_private_tests
    SOURCES
        ../common/qbackendnodetester.cpp ../common/qbackendnodetester.h
        ../common/testarbiter.h
)
