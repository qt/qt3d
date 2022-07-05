# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

# Generated from qabstractaxisinput.pro.

#####################################################################
## tst_qabstractaxisinput Test:
#####################################################################

qt_internal_add_test(tst_qabstractaxisinput
    SOURCES
        ../commons/testdevice.h
        ../commons/testdevice.cpp
        ../commons/testdeviceproxy.h
        tst_qabstractaxisinput.cpp
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

#### Keys ignored in scope 1:.:.:qabstractaxisinput.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_internal_extend_target(tst_qabstractaxisinput CONDITION QT_FEATURE_private_tests
    SOURCES
        ../../core/common/qbackendnodetester.cpp ../../core/common/qbackendnodetester.h
        ../../core/common/testarbiter.h
)
