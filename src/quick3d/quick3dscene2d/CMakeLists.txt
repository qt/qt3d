# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

qt_internal_add_qml_module(3DQuickScene2D
    URI "QtQuick.Scene2D"
    VERSION "${PROJECT_VERSION}"
    PAST_MAJOR_VERSIONS 2
    DEPENDENCIES
        Qt3D.Core/auto
        Qt3D.Render/auto
    NO_GENERATE_PLUGIN_SOURCE
    CLASS_NAME QtQuickScene2DPlugin
    PLUGIN_TARGET qtquickscene2dplugin
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

qt_internal_extend_target(3DQuickScene2D CONDITION gcov
    COMPILE_OPTIONS
        -fprofile-arcs
        -ftest-coverage
    LINK_OPTIONS
        "-fprofile-arcs"
        "-ftest-coverage"
)

qt_internal_extend_target(qtquickscene2dplugin
    SOURCES
        qtquickscene2dplugin.h qtquickscene2dplugin.cpp
    LIBRARIES
        Qt::3DRenderPrivate
        Qt::Core
        Qt::Qml
)
