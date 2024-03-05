# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

qt_examples_build_begin(EXTERNAL_BUILD)

if(QT_FEATURE_qt3d_extras)
    add_subdirectory(qt3d)
endif()

qt_examples_build_end()
