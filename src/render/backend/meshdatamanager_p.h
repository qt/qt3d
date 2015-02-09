/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_RENDER_MESHDATAMANAGER_H
#define QT3D_RENDER_MESHDATAMANAGER_H

#include <Qt3DRenderer/qabstractmesh.h>
#include <Qt3DRenderer/qmeshdata.h>
#include <Qt3DRenderer/private/rendermesh_p.h>
#include <Qt3DRenderer/private/handle_types_p.h>

#include <Qt3DCore/private/qresourcemanager_p.h>

#include <QHash>
#include <QPair>
#include <QString>
#include <Qt3DCore/qnodeid.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

class MeshDataManager : public QResourceManager<QMeshData,
                                                 QNodeId,
                                                 16,
                                                 Qt3D::ArrayAllocatingPolicy,
                                                 Qt3D::ObjectLevelLockingPolicy>
{
public:
    MeshDataManager() {}
    void addMeshData(QAbstractMeshFunctorPtr functor, const QNodeId &meshUuid);

    QHash<QNodeId, QAbstractMeshFunctorPtr> meshesPending();
    HMeshData meshDataFromFunctor(QAbstractMeshFunctorPtr functor) const;
    void addMeshDataForFunctor(HMeshData meshDataHandle, QAbstractMeshFunctorPtr functor);

private:
    // List of meshes that we need to schedule jobs to load
    // and calculate bounds for.

    QHash<QNodeId, QAbstractMeshFunctorPtr> m_meshesPending;
    QHash<QAbstractMeshFunctorPtr, HMeshData> m_meshFunctors;
};

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_MESHDATAMANAGER_H
