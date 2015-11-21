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

#include "updateboundingvolumejob_p.h"

#include <Qt3DRender/private/renderer_p.h>
#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/sphere_p.h>

#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

namespace {

void expandWorldBoundingVolume(Qt3DRender::Render::Entity *node)
{
    // Go to the nodes that have the most depth
    Q_FOREACH (Entity *c, node->children())
        expandWorldBoundingVolume(c);

    // Then traverse back from leaf to root
    // Initialize parent bounding volume to be equal to that of the first child
    if (node->hasChildren()) {
        Qt3DRender::Render::Sphere *parentBoundingVolume = node->worldBoundingVolumeWithChildren();
        Q_FOREACH (Entity *c, node->children())
            parentBoundingVolume->expandToContain(*c->worldBoundingVolumeWithChildren());
    }
}

}

UpdateBoundingVolumeJob::UpdateBoundingVolumeJob()
    : m_node(Q_NULLPTR)
{
}

void UpdateBoundingVolumeJob::setRoot(Entity *root)
{
    m_node = root;
}

void UpdateBoundingVolumeJob::run()
{
    // Expand worldBoundingVolumeWithChildren of each node that has children by the
    // bounding volumes of the children.

    // TODO: Implement this using a parallel_for
    qCDebug(Jobs) << "Entering" << Q_FUNC_INFO << QThread::currentThread();
    expandWorldBoundingVolume(m_node);
    qCDebug(Jobs) << "Exiting" << Q_FUNC_INFO << QThread::currentThread();
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
