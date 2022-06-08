// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3D_QUICK_FOREIGN_P_H
#define QT3D_QUICK_FOREIGN_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtQml/qqml.h>
#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/qtransform.h>
#include <Qt3DCore/qarmature.h>
#include <Qt3DCore/qabstractskeleton.h>
#include <Qt3DCore/qskeletonloader.h>
#include <Qt3DCore/qattribute.h>
#include <Qt3DCore/qbuffer.h>
#include <Qt3DCore/qgeometry.h>
#include <Qt3DCore/qgeometryview.h>
#include <Qt3DCore/qboundingvolume.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qjoint.h>

#include <private/quick3dentity_p.h>
#include <private/quick3dgeometry_p.h>
#include <private/quick3dnode_p.h>
#include <private/quick3djoint_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
namespace Quick {

struct QComponentForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(Component3D)
    QML_ADDED_IN_VERSION(2, 0);
    QML_FOREIGN(Qt3DCore::QComponent)
};

struct QEntityForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(Entity)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DCore::QEntity)
    QML_EXTENDED(Qt3DCore::Quick::Quick3DEntity)
};

struct QTransformForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(Transform)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DCore::QTransform)
};

struct QArmatureForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(Armature)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DCore::QArmature)
};

struct QAbstractSkeletonForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(AbstractSkeleton)
    QML_ADDED_IN_VERSION(2, 10)
    QML_FOREIGN(Qt3DCore::QAbstractSkeleton)
    QML_UNCREATABLE("AbstractSkeleton is an abstract base class")
};

struct QSkeletonLoaderForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(SkeletonLoader)
    QML_ADDED_IN_VERSION(2, 10)
    QML_FOREIGN(Qt3DCore::QSkeletonLoader)
};

struct QAttributeForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(Attribute)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DCore::QAttribute)
};

struct QBufferForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(BufferBase)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DCore::QBuffer)
    QML_UNCREATABLE("Use Quick3DBuffer in QML")
};

struct QGeometryForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(Geometry)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DCore::QGeometry)
    QML_EXTENDED(Qt3DCore::Quick::Quick3DGeometry)
};

struct QGeometryViewForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(GeometryView)
    QML_ADDED_IN_VERSION(2, 16)
    QML_FOREIGN(Qt3DCore::QGeometryView)
};

struct QBoundingVolumeForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(BoundingVolume)
    QML_ADDED_IN_VERSION(2, 16)
    QML_FOREIGN(Qt3DCore::QBoundingVolume)
};

// Ideally we want to make Node an uncreatable type
// TODO: We can do that in a civilized way now: QML_UNCREATABLE("foo bar")
//       It might cause some incompatibility, though.
struct QNodeForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(Node)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DCore::QNode)
    QML_EXTENDED(Qt3DCore::Quick::Quick3DNode)
};

struct QJointForeign
{
    Q_GADGET
    QML_NAMED_ELEMENT(Joint)
    QML_ADDED_IN_VERSION(2, 0)
    QML_FOREIGN(Qt3DCore::QJoint)
    QML_EXTENDED(Qt3DCore::Quick::Quick3DJoint)
};

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3D_QUICK_FOREIGN_P_H
