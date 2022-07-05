# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

# Generated from qskeletonmapping.pro.

#####################################################################
## tst_qskeletonmapping Test:
#####################################################################

qt_internal_add_test(tst_qskeletonmapping
    SOURCES
        tst_qskeletonmapping.cpp
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

#### Keys ignored in scope 1:.:.:qskeletonmapping.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_internal_extend_target(tst_qskeletonmapping CONDITION QT_FEATURE_private_tests
    SOURCES
        ../../core/common/qbackendnodetester.cpp ../../core/common/qbackendnodetester.h
        ../../core/common/testarbiter.h
)
