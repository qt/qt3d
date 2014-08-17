/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_RENDER_MESHDATAMANAGER_H
#define QT3D_RENDER_MESHDATAMANAGER_H

#include <Qt3DCore/qabstractmesh.h>
#include <Qt3DCore/qresourcesmanager.h>
#include <Qt3DRenderer/meshdata.h>
#include <Qt3DRenderer/private/rendermesh_p.h>

#include <QHash>
#include <QPair>
#include <QString>
#include <QUuid>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

typedef QHandle<MeshData, 16> HMeshData;
typedef QHandle<RenderMesh, 16> HMesh;

class MeshDataManager : public QResourcesManager<MeshData,
                                                 QUuid,
                                                 16,
                                                 Qt3D::ListAllocatingPolicy,
                                                 Qt3D::ObjectLevelLockingPolicy>
{
public:
    MeshDataManager();

    inline bool hasMeshData(const QUuid &id) { return contains(id); }
    inline MeshData* getOrCreateMeshData(const QUuid &id) { return getOrCreateResource(id); }
    inline MeshData* meshData(const QUuid &id) { return lookupResource(id); }
    void addMeshData(QAbstractMeshFunctorPtr functor, const QUuid &meshUuid);

    QHash<QUuid, QAbstractMeshFunctorPtr> meshesPending();

private:
    // List of meshes that we need to schedule jobs to load
    // and calculate bounds for.

    QHash<QUuid, QAbstractMeshFunctorPtr> m_meshesPending;
};

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_MESHDATAMANAGER_H
