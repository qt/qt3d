// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "buffermanager_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

BufferManager::BufferManager()
{
}

BufferManager::~BufferManager()
{
}

void BufferManager::addDirtyBuffer(Qt3DCore::QNodeId bufferId)
{
    if (!m_dirtyBuffers.contains(bufferId))
        m_dirtyBuffers.push_back(bufferId);
}

QList<Qt3DCore::QNodeId> BufferManager::takeDirtyBuffers()
{
    return qMove(m_dirtyBuffers);
}

// Called in QAspectThread::syncChanges
void BufferManager::removeBufferReference(Qt3DCore::QNodeId bufferId)
{
    QMutexLocker lock(&m_mutex);
    Q_ASSERT(m_bufferReferences.contains(bufferId) && m_bufferReferences[bufferId] > 0);
    m_bufferReferences[bufferId]--;
}

// Called in QAspectThread
void BufferManager::addBufferReference(Qt3DCore::QNodeId bufferId)
{
    QMutexLocker lock(&m_mutex);
    m_bufferReferences[bufferId]++;
}

// Called in Render thread
QList<Qt3DCore::QNodeId> BufferManager::takeBuffersToRelease()
{
    QMutexLocker lock(&m_mutex);
    QList<Qt3DCore::QNodeId> buffersToRelease;
    for (auto it = m_bufferReferences.begin(), end = m_bufferReferences.end(); it != end; /*erasing*/) {
        if (it.value() == 0) {
            buffersToRelease.append(it.key());
            it = m_bufferReferences.erase(it);
        } else {
            ++it;
        }
    }
    return buffersToRelease;
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
