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

#include "loadmeshdatajob.h"

#include <objloader.h>
#include <qattribute.h>
#include <qmesh.h>
#include <sphere.h>
#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/meshdatamanager_p.h>
#include <Qt3DRenderer/private/meshmanager_p.h>

#include <QThread>
#include "renderlogging.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

LoadMeshDataJob::LoadMeshDataJob(QAbstractMeshFunctorPtr functor, const QUuid &meshUuid)
    : QJob()
    , m_meshUuid(meshUuid)
    , m_functor(functor)
{
}

void LoadMeshDataJob::run()
{
    qCDebug(Jobs) << "Entering" << Q_FUNC_INFO << QThread::currentThread();

    if (m_functor.isNull())
        return ;
    // Load the mesh from disk (or wherever)
    QMeshDataPtr meshDataPtr = m_functor->operator ()().staticCast<QMeshData>();
    if (meshDataPtr.isNull()) {
        qCDebug(Jobs) << Q_FUNC_INFO << "Mesh has no raw data";
        return ;
    }

    // TO DO try to use QAbstractMeshData if possible
    QMutexLocker lock(m_renderer->mutex());
    QMeshData *meshData = m_renderer->meshDataManager()->getOrCreateResource(m_meshUuid);
    *meshData = *(meshDataPtr.data());

    AttributePtr attr = meshData->attributeByName(QAbstractMeshData::defaultPositionAttributeName()).staticCast<Attribute>();
    if (!attr)
        qCWarning(Jobs) << Q_FUNC_INFO << "unknown attribute: position";
}

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE
