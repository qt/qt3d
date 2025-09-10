// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "buffer_p.h"
#include <Qt3DCore/private/qbuffer_p.h>
#include <Qt3DRender/private/buffermanager_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

Buffer::Buffer()
    : BackendNode(QBackendNode::ReadWrite)
    , m_usage(Qt3DCore::QBuffer::StaticDraw)
    , m_bufferDirty(false)
    , m_access(Qt3DCore::QBuffer::Write)
    , m_manager(nullptr)
{
    // Maybe it could become read write if we want to inform
    // the frontend QBuffer node of any backend issue
}

Buffer::~Buffer()
{
}

void Buffer::cleanup()
{
    m_usage = Qt3DCore::QBuffer::StaticDraw;
    m_data.clear();
    m_bufferUpdates.clear();
    m_bufferDirty = false;
    m_access = Qt3DCore::QBuffer::Write;
}


void Buffer::setManager(BufferManager *manager)
{
    m_manager = manager;
}

//Called from th sendBufferJob
void Buffer::updateDataFromGPUToCPU(QByteArray data)
{
    // Note: when this is called, data is what's currently in GPU memory
    // so m_data shouldn't be reuploaded
    m_data = data;
}

void Buffer::forceDataUpload()
{
    // We push back an update with offset = -1
    // As this is the way to force data to be loaded
    Qt3DCore::QBufferUpdate updateNewData;
    updateNewData.offset = -1;
    m_bufferUpdates.clear(); //previous updates are pointless
    m_bufferUpdates.push_back(updateNewData);
}

void Buffer::syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime)
{
    BackendNode::syncFromFrontEnd(frontEnd, firstTime);
    const Qt3DCore::QBuffer *node = qobject_cast<const Qt3DCore::QBuffer *>(frontEnd);
    if (!node)
        return;

    if (firstTime && m_manager != nullptr) {
        m_manager->addBufferReference(peerId());
        m_bufferDirty = true;
    }

    m_access = node->accessType();
    if (m_usage != node->usage()) {
        m_usage = node->usage();
        m_bufferDirty = true;
    }
    {
        const QVariant v = node->property(Qt3DCore::QBufferPrivate::UpdateDataPropertyName);

        // Make sure we record data if it's the first time we are called
        // or if we have no partial updates
        if (firstTime || !v.isValid()){
            const QByteArray newData = node->data();
            const bool dirty = m_data != newData;
            m_bufferDirty |= dirty;
            m_data = newData;

            // Since frontend applies partial updates to its m_data
            // if we enter this code block, there's no problem in actually
            // ignoring the partial updates
            if (v.isValid())
                const_cast<Qt3DCore::QBuffer *>(node)->setProperty(Qt3DCore::QBufferPrivate::UpdateDataPropertyName, {});

            if (dirty && !m_data.isEmpty())
                forceDataUpload();
        } else if (v.isValid()) {
            // Apply partial updates and record them to allow partial upload to the GPU
            const QVariantList updateList = v.toList();
            for (const QVariant &update : updateList) {
                Qt3DCore::QBufferUpdate updateData = update.value<Qt3DCore::QBufferUpdate>();
                m_data.replace(updateData.offset, updateData.data.size(), updateData.data);
                m_bufferUpdates.push_back(updateData);
                m_bufferDirty = true;
            }

            const_cast<Qt3DCore::QBuffer *>(node)->setProperty(Qt3DCore::QBufferPrivate::UpdateDataPropertyName, {});
        }
    }
    markDirty(AbstractRenderer::BuffersDirty);
}

// Called by Renderer once the buffer has been uploaded to OpenGL
void Buffer::unsetDirty()
{
    m_bufferDirty = false;
}

BufferFunctor::BufferFunctor(AbstractRenderer *renderer, BufferManager *manager)
    : m_manager(manager)
    , m_renderer(renderer)
{
}

Qt3DCore::QBackendNode *BufferFunctor::create(Qt3DCore::QNodeId id) const
{
    Buffer *buffer = m_manager->getOrCreateResource(id);
    buffer->setManager(m_manager);
    buffer->setRenderer(m_renderer);
    return buffer;
}

Qt3DCore::QBackendNode *BufferFunctor::get(Qt3DCore::QNodeId id) const
{
    return m_manager->lookupResource(id);
}

void BufferFunctor::destroy(Qt3DCore::QNodeId id) const
{
    if (m_manager->contains(id)) {
        m_manager->removeBufferReference(id);
        m_manager->releaseResource(id);
    }
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
