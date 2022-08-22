# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from qmouseevent.pro.

#####################################################################
## tst_qmouseevent Test:
#####################################################################

#special case begin
qt_internal_add_test(tst_qt3d_qmouseevent
#special case end
    SOURCES
        ../commons/testdevice.h
        ../commons/testdevice.cpp
        ../commons/testdeviceproxy.h
        tst_qmouseevent.cpp
    INCLUDE_DIRECTORIES
        ../../core/common
        ../commons
    LIBRARIES
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

#special case begin
qt_internal_extend_target(tst_qt3d_qmouseevent CONDITION QT_FEATURE_private_tests
#special case end
    SOURCES
        ../../core/common/qbackendnodetester.cpp ../../core/common/qbackendnodetester.h
        ../../core/common/testarbiter.h
)
