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

#ifndef QT3D_RENDER_RENDERMESH_H
#define QT3D_RENDER_RENDERMESH_H

#include <Qt3DCore/qaxisalignedboundingbox.h>
#include <Qt3DRenderer/private/handle_types_p.h>
#include <Qt3DCore/qbackendnode.h>

QT_BEGIN_NAMESPACE

class QReadWriteLock;

namespace Qt3D {

class QAbstractMesh;
class QAbstractMeshFunctor;
class QRenderPass;
class QMeshData;

typedef QSharedPointer<QAbstractMeshFunctor> QAbstractMeshFunctorPtr;

namespace Render {

class MeshManager;
class MeshDataManager;
class RenderEntity;
class RenderMaterial;
class RenderTechnique;

class Q_AUTOTEST_EXPORT RenderMesh : public QBackendNode
{
public:
    RenderMesh();
    ~RenderMesh();
    void cleanup();

    void updateFromPeer(QNode *peer) Q_DECL_OVERRIDE;

    void sceneChangeEvent(const QSceneChangePtr &e) Q_DECL_OVERRIDE;
    HMeshData meshDataHandle() const;
    QMeshData *meshData() const;
    void setMeshData(HMeshData handle);
    void setMeshDataManager(MeshDataManager *manager);

    QAbstractMeshFunctorPtr meshFunctor() const { return m_functor; }
    inline bool isEnabled() const { return m_enabled; }

private:
    QAbstractMeshFunctorPtr m_functor;

    bool m_meshDirty;
    HMeshData m_meshDataHandle;
    MeshDataManager *m_meshDataManager;
    bool m_enabled;

    void setMeshFunctor(QAbstractMeshFunctorPtr);
};

class RenderMeshCreatorFunctor : public QBackendNodeFunctor
{
public:
    explicit RenderMeshCreatorFunctor(MeshManager *meshManager, MeshDataManager *meshDataManager);
    QBackendNode *create(QNode *frontend, const QBackendNodeFactory *factory) const Q_DECL_OVERRIDE;
    QBackendNode *get(const QNodeId &id) const Q_DECL_OVERRIDE;
    void destroy(const QNodeId &id) const Q_DECL_OVERRIDE;

private:
    MeshManager *m_meshManager;
    MeshDataManager *m_meshDataManager;
};

} // Render
} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERMESH_H
