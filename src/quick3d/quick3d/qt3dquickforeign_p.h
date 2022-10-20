/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
