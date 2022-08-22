# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from boundingvolumedebug.pro.

#####################################################################
## boundingvolumedebug Test:
#####################################################################

qt_internal_add_test(boundingvolumedebug
    SOURCES
        ../commons/testrenderer.cpp ../commons/testrenderer.h
        tst_boundingvolumedebug.cpp
    INCLUDE_DIRECTORIES
        ../commons
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
)

#### Keys ignored in scope 1:.:.:boundingvolumedebug.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_internal_extend_target(boundingvolumedebug CONDITION useCommonTestAspect
    SOURCES
        ../commons/testaspect.cpp ../commons/testaspect.h
)
