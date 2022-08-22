# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from quick3dscene2d.pro.

#####################################################################
## Qt::3DQuickScene2D Module:
#####################################################################

qt_internal_add_module(3DQuickScene2D
    SOURCES
        items/qscene2d.cpp items/qscene2d.h items/qscene2d_p.h
        items/qt3dquick3dscene2d.cpp items/qt3dquick3dscene2d_p.h
        items/scene2d.cpp items/scene2d_p.h
        items/scene2devent_p.h
        items/scene2dmanager.cpp items/scene2dmanager_p.h
        items/scene2dsharedobject.cpp items/scene2dsharedobject_p.h
        qt3dquickscene2d_global.cpp qt3dquickscene2d_global.h qt3dquickscene2d_global_p.h
        qt3dquickscene2d_logging.cpp qt3dquickscene2d_logging_p.h
        qt3dquickscene2dnodefactory.cpp qt3dquickscene2dnodefactory_p.h
    INCLUDE_DIRECTORIES
        items
    LIBRARIES
        Qt::3DCorePrivate
        Qt::3DQuickPrivate
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::QmlPrivate
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DQuick
        Qt::3DRender
        Qt::Core
        Qt::Gui
        Qt::Qml
    PRIVATE_MODULE_INTERFACE
        Qt::3DCorePrivate
        Qt::3DQuickPrivate
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::QmlPrivate
)

#### Keys ignored in scope 1:.:.:quick3dscene2d.pro:<TRUE>:
# MODULE = "3dquickscene2d"

## Scopes:
#####################################################################

qt_internal_extend_target(3DQuickScene2D CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)
