# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

# Generated from matrix4x4_sse.pro.

#####################################################################
## tst_matrix4x4_sse Test:
#####################################################################

qt_internal_add_test(tst_matrix4x4_sse
    SOURCES
        tst_matrix4x4_sse.cpp
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::Gui
)
