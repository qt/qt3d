# Generated from qmouseevent.pro.

#####################################################################
## tst_qmouseevent Test:
#####################################################################

qt_internal_add_test(tst_qmouseevent
    SOURCES
        ../commons/testdevice.h
        ../commons/testdeviceproxy.h
        tst_qmouseevent.cpp
    INCLUDE_DIRECTORIES
        ../../core/common
        ../commons
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DInput
        Qt::3DInputPrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:qmouseevent.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_internal_extend_target(tst_qmouseevent CONDITION QT_FEATURE_private_tests
    SOURCES
        ../../core/common/qbackendnodetester.cpp ../../core/common/qbackendnodetester.h
        ../../core/common/testarbiter.h
)
