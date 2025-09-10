// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_BUFFERMANAGER_H
#define QT3DRENDER_RENDER_BUFFERMANAGER_H

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

#include <Qt3DCore/private/qresourcemanager_p.h>
#include <Qt3DRender/private/buffer_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT BufferManager : public Qt3DCore::QResourceManager<
        Buffer,
        Qt3DCore::QNodeId,
        Qt3DCore::ObjectLevelLockingPolicy>
{
public:
    BufferManager();
    ~BufferManager();

    // Aspect Thread
    void addDirtyBuffer(Qt3DCore::QNodeId bufferId);
    QList<Qt3DCore::QNodeId> takeDirtyBuffers();

    // Aspect Thread
    void addBufferReference(Qt3DCore::QNodeId bufferId);
    void removeBufferReference(Qt3DCore::QNodeId bufferId);

    // Render Thread (no concurrent access)
    QList<Qt3DCore::QNodeId> takeBuffersToRelease();

private:
    QList<Qt3DCore::QNodeId> m_dirtyBuffers;
    QHash<Qt3DCore::QNodeId, int> m_bufferReferences;
    QMutex m_mutex;
};

} // namespace Render
} // namespace Qt3DRender

Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Buffer, Q_REQUIRES_CLEANUP)

QT_END_NAMESPACE


#endif // QT3DRENDER_RENDER_BUFFERMANAGER_H
