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

#include "meshdatamanager_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

// Called by aspect thread when RenderMesh receive a new functor in syncChanges
void MeshDataManager::addMeshData(QAbstractMeshFunctorPtr functor, const QNodeId &meshUuid)
{
    m_meshesPending[meshUuid] = functor;
}

// Called by single thread in QRenderAspect
// Needs to be protected as we ways call it while addMeshData is called
QHash<QNodeId, QAbstractMeshFunctorPtr> MeshDataManager::meshesPending()
{
    QHash<QNodeId, QAbstractMeshFunctorPtr> meshFunctors = m_meshesPending;
    m_meshesPending.clear();
    return meshFunctors;
}

HMeshData MeshDataManager::meshDataFromFunctor(QAbstractMeshFunctorPtr functor) const
{
    QHash<QAbstractMeshFunctorPtr, HMeshData>::const_iterator it = m_meshFunctors.begin();
    const QHash<QAbstractMeshFunctorPtr, HMeshData>::const_iterator end = m_meshFunctors.end();

    while (it != end) {
        if (*it.key() == *functor)
            return it.value();
        ++it;
    }
    return HMeshData();
}

void MeshDataManager::addMeshDataForFunctor(HMeshData meshDataHandle, QAbstractMeshFunctorPtr functor)
{
    m_meshFunctors.insert(functor, meshDataHandle);
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
