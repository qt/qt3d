# Generated from qjoint.pro.

#####################################################################
## tst_qjoint Test:
#####################################################################

qt_add_test(tst_qjoint
    SOURCES
        tst_qjoint.cpp
    INCLUDE_DIRECTORIES
        ../common
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:qjoint.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_extend_target(tst_qjoint CONDITION QT_FEATURE_private_tests
    SOURCES
        ../common/qbackendnodetester.cpp ../common/qbackendnodetester.h
        ../common/testarbiter.h
)
