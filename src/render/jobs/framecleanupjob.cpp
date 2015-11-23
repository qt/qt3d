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

#include "framecleanupjob_p.h"
#include <private/renderer_p.h>
#include <private/nodemanagers_p.h>
#include <private/entity_p.h>
#include <private/shaderdata_p.h>
#include <private/managers_p.h>
#include <private/texturedatamanager_p.h>
#include <private/sphere_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

FrameCleanupJob::FrameCleanupJob(NodeManagers *managers)
    : m_managers(managers)
    , m_root(Q_NULLPTR)
{
}

FrameCleanupJob::~FrameCleanupJob()
{
}

void FrameCleanupJob::setRoot(Entity *root)
{
    m_root = root;
}

void FrameCleanupJob::run()
{
    // mark each ShaderData clean
    ShaderData::cleanup(m_managers);

    // Cleanup texture handles
    TextureDataManager *textureDataManager = m_managers->textureDataManager();
    textureDataManager->cleanup();

    // Debug bounding volume debug
    updateBoundingVolumesDebug(m_root);
}

void FrameCleanupJob::updateBoundingVolumesDebug(Entity *node)
{
    BoundingVolumeDebug *debugBV = node->renderComponent<BoundingVolumeDebug>();
    if (debugBV) {
        Qt3DRender::Render::Sphere s;
        if (!debugBV->isRecursive()) {
            s = *node->worldBoundingVolume();
        } else {
            s = *node->worldBoundingVolumeWithChildren();
        }
        debugBV->setRadius(s.radius());
        debugBV->setCenter(s.center());
    }

    Q_FOREACH (Entity *c, node->children())
        updateBoundingVolumesDebug(c);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
