# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from rendercapture-cpp.pro.

#####################################################################
## rendercapture-cpp Binary:
#####################################################################

qt_internal_add_manual_test(rendercapture-cpp
    GUI
    SOURCES
        main.cpp
        mycapture.h
        orbittransformcontroller.cpp orbittransformcontroller.h
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DRender
        Qt::Gui
        Qt::Widgets
)

## Scopes:
#####################################################################
