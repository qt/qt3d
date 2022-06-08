// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


import Qt3D.Core 2.0 as QQ3Core20
import Qt3D.Core 2.9 as QQ3Core29
import Qt3D.Core 2.10 as QQ3Core210
import Qt3D.Core 2.14 as QQ3Core214
import QtQuick 2.0

Item {

    // Misc
    //QQ3Core20.Component3D                  // (uncreatable) Qt3DCore::QComponent
    QQ3Core20.Entity {}                      //Qt3DCore::QEntity, Qt3DCore::Quick::Quick3DEntity
    QQ3Core20.EntityLoader {}                //Qt3DCore::Quick::Quick3DEntityLoader
    QQ3Core20.NodeInstantiator {}            //Qt3DCore::Quick::Quick3DNodeInstantiator
    QQ3Core20.Transform {}                   //Qt3DCore::QTransform
    QQ3Core20.QuaternionAnimation {}         //Qt3DCore::Quick::QQuaternionAnimation

    QQ3Core29.Entity {}                      //Qt3DCore::QEntity, Qt3DCore::Quick::Quick3DEntity

    QQ3Core210.Armature {}
    QQ3Core210.SkeletonLoader {}
    QQ3Core210.Joint {}

    QQ3Core214.Transform {}

    QQ3Core20.Attribute {}                   //Qt3DCore::QAttribute
    QQ3Core20.Buffer {}                      //Qt3DCore::Quick::Quick3DBuffer
    QQ3Core20.Geometry {}                    //Qt3DCore::QGeometry, Qt3DCore::Quick::Quick3DGeometry
}
