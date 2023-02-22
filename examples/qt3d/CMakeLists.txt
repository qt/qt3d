# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

qt_internal_add_example(3d-text)
qt_internal_add_example(simple-cpp)
if(TARGET Qt::Widgets)
    qt_internal_add_example(basicshapes-cpp)
endif()
if(TARGET Qt::Quick)
    qt_internal_add_example(controls)
    qt_internal_add_example(instanced-arrays-qml)
    qt_internal_add_example(multiviewport)
    qt_internal_add_example(simple-qml)
    qt_internal_add_example(simplecustommaterial)

    # these require qt6_add_big_resources but it's broken at this point QTBUG-85051
#    qt_internal_add_example(lights)
#    qt_internal_add_example(pbr-materials)
#    qt_internal_add_example(wireframe)
endif()
