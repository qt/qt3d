/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

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
    QVector<Qt3DCore::QNodeId> takeDirtyBuffers();

    // Aspect Thread
    void addBufferReference(Qt3DCore::QNodeId bufferId);
    void removeBufferReference(Qt3DCore::QNodeId bufferId);

    // Render Thread (no concurrent access)
    QVector<Qt3DCore::QNodeId> takeBuffersToRelease();

private:
    QVector<Qt3DCore::QNodeId> m_dirtyBuffers;
    QHash<Qt3DCore::QNodeId, int> m_bufferReferences;
    QMutex m_mutex;
};

} // namespace Render
} // namespace Qt3DRender

Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::Buffer, Q_REQUIRES_CLEANUP)

QT_END_NAMESPACE


#endif // QT3DRENDER_RENDER_BUFFERMANAGER_H
