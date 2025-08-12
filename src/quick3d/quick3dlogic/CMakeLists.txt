# Copyright (C) 2025 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

qt_internal_add_qml_module(3DQuickLogic
    URI "Qt3D.Logic"
    VERSION "${PROJECT_VERSION}"
    PLUGIN_TARGET quick3dlogicplugin
    CLASS_NAME Qt3DQuick3DLogicPlugin
    PAST_MAJOR_VERSIONS 2
    SOURCES
        qt3dquick3dlogicforeign_p.h
    DEPENDENCIES
        Qt3D.Core
    LIBRARIES
        Qt::3DCore
        3DLogic
        Qt::Core
        Qt::CorePrivate
        Qt::Gui
        Qt::Qml
)
