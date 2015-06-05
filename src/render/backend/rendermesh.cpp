/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
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

#include "rendermesh_p.h"
#include "meshdatamanager_p.h"
#include "qmesh.h"

#include <Qt3DRenderer/private/qgraphicscontext_p.h>
#include <Qt3DRenderer/private/managers_p.h>
#include <Qt3DRenderer/qmeshdata.h>
#include <Qt3DCore/qscenepropertychange.h>

#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

/*!
 * \class RenderMesh
 * \internal
 *
 * Monitor a frontend Mesh for source changes. If the source is changed,
 * the QMeshData returned is either a valid QMeshData corresponding to the source
 * or null if the new QMeshData hasn't been loaded. If this is the case the meshDirty
 * flag is set to true.
 *
 * \sa QMeshData
 */

RenderMesh::RenderMesh()
    : QBackendNode()
    , m_meshDirty(true)
    , m_meshDataManager(Q_NULLPTR)
    , m_enabled(true)
{
}

RenderMesh::~RenderMesh()
{
    cleanup();
}

void RenderMesh::cleanup()
{
}

void RenderMesh::updateFromPeer(QNode *peer)
{
    QAbstractMesh *mesh = static_cast<QAbstractMesh *>(peer);
    m_meshDirty = true;
    m_enabled = mesh->isEnabled();
    setMeshFunctor(mesh->meshFunctor());
}

void RenderMesh::sceneChangeEvent(const QSceneChangePtr &e)
{
    switch (e->type()) {
    case NodeUpdated: {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("meshFunctor")) // Mesh with source
            setMeshFunctor(propertyChange->value().value<QAbstractMeshFunctorPtr>());
        else if (propertyChange->propertyName() == QByteArrayLiteral("enabled"))
            m_enabled = propertyChange->value().toBool();
        break;
    }

    default:
        break;
    }
}

QMeshData *RenderMesh::meshData() const
{
    Q_ASSERT(m_meshDataManager);
    return m_meshDataManager->data(m_meshDataHandle);
}

HMeshData RenderMesh::meshDataHandle() const
{
    return m_meshDataHandle;
}

void RenderMesh::setMeshData(HMeshData handle)
{
    m_meshDataHandle = handle;
}

void RenderMesh::setMeshDataManager(MeshDataManager *manager)
{
    if (m_meshDataManager != manager)
        m_meshDataManager = manager;
}

void RenderMesh::setMeshFunctor(QAbstractMeshFunctorPtr functor)
{
    if (m_functor != functor) {
        m_functor = functor;
        m_meshDataManager->addMeshData(m_functor, peerUuid());
    }
}

RenderMeshCreatorFunctor::RenderMeshCreatorFunctor(MeshManager *meshManager, MeshDataManager *meshDataManager)
    : m_meshManager(meshManager)
    , m_meshDataManager(meshDataManager)
{
}

QBackendNode *RenderMeshCreatorFunctor::create(QNode *frontend, const QBackendNodeFactory *factory) const
{
    RenderMesh *mesh = m_meshManager->getOrCreateResource(frontend->id());
    mesh->setFactory(factory);
    mesh->setMeshDataManager(m_meshDataManager);
    mesh->setPeer(frontend);
    return mesh;
}

QBackendNode *RenderMeshCreatorFunctor::get(const QNodeId &id) const
{
    return m_meshManager->lookupResource(id);
}

void RenderMeshCreatorFunctor::destroy(const QNodeId &id) const
{
    m_meshManager->releaseResource(id);
}

} // Render
} // Qt3D

QT_END_NAMESPACE
