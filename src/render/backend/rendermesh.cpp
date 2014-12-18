/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
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
    setMeshFunctor(mesh->meshFunctor());
}

void RenderMesh::sceneChangeEvent(const QSceneChangePtr &e)
{
    switch (e->type()) {
    case NodeUpdated: {
        QScenePropertyChangePtr propertyChange = qSharedPointerCast<QScenePropertyChange>(e);
        if (propertyChange->propertyName() == QByteArrayLiteral("meshFunctor")) // Mesh with source
            setMeshFunctor(propertyChange->value().value<QAbstractMeshFunctorPtr>());
        break;
    }

    default:
        break;
    }
}

HMeshData RenderMesh::meshData() const
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

QBackendNode *RenderMeshCreatorFunctor::create(QNode *frontend) const
{
    RenderMesh *mesh = m_meshManager->getOrCreateResource(frontend->id());
    mesh->setMeshDataManager(m_meshDataManager);
    mesh->setPeer(frontend);
    return mesh;
}

QBackendNode *RenderMeshCreatorFunctor::get(QNode *frontend) const
{
    return m_meshManager->lookupResource(frontend->id());
}

void RenderMeshCreatorFunctor::destroy(QNode *frontend) const
{
    m_meshManager->releaseResource(frontend->id());
}

} // Render
} // Qt3D

QT_END_NAMESPACE
