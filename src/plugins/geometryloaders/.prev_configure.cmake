# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause



#### Inputs



#### Libraries



#### Tests



#### Features

qt_feature("qt3d-fbxsdk" PRIVATE
    LABEL "Autodesk FBX"
    CONDITION libs.fbx OR FIXME
)
qt_feature_definition("qt3d-fbxsdk" "QT_3DGEOMETRYLOADERS_FBX")
qt_configure_add_summary_section(NAME "Qt 3D GeometryLoaders")
qt_configure_add_summary_entry(ARGS "qt3d-fbxsdk")
qt_configure_end_summary_section() # end of "Qt 3D GeometryLoaders" section
